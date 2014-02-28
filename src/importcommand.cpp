#include "importcommand.h"
#include "collectionresolvejob.h"
#include "xml/xmldocument.h"

#include <akonadi/collectioncreatejob.h>
#include <akonadi/collectionfetchjob.h>

#include <akonadi/itemcreatejob.h>
#include <akonadi/collectionfetchscope.h>

#include <kcmdlineargs.h>
#include <kjob.h>
#include <kurl.h>

#include <iostream>

using namespace Akonadi;

ImportCommand::ImportCommand(QObject *parent)
    : AbstractCommand(parent), mResolveJob(0)
{
    mShortHelp = ki18nc("@info:shell", "Import collections and items from an XML file").toString();
}

ImportCommand::~ImportCommand()
{

}

void ImportCommand::setupCommandOptions(KCmdLineOptions &options)
{
    AbstractCommand::setupCommandOptions(options);
    options.add("+file", ki18nc("@info:shell", "The file to import from"));
    options.add("p").add("parent <collection>", ki18nc("@info:shell", "Parent collection"));
    options.add("m").add("merge <collection>", ki18nc("@info:shell", "Merge collection"));
}

int ImportCommand::initCommand(KCmdLineArgs *parsedArgs)
{
    if (parsedArgs->count() < 2) {
        emitErrorSeeHelp(ki18nc("@info:shell", "No file specified"));
        return InvalidUsage;
    } else if (parsedArgs->isSet("parent") && parsedArgs->isSet("merge")) {
        emitErrorSeeHelp(ki18nc("@info:shell", "Please specify one of either parent or merge options"));
        return InvalidUsage;
    }

    mImportFile = parsedArgs->arg(1);
    mMergeCollection = parsedArgs->isSet("merge");

    if (!mMergeCollection) {
        emit error("Not yet implemented");
        return InvalidUsage;
    }

    mParentCollectionArg = (mMergeCollection) ? parsedArgs->getOption("merge") : parsedArgs->getOption("parent");

    mXmlDocument = new XmlDocument(mImportFile);

    if (!mXmlDocument->isValid()) {
        emit error(mXmlDocument->lastError());
        return RuntimeError;
    }
    mCollections = mXmlDocument->collections();

    return NoError;
}

void ImportCommand::start()
{
    mExitStatus = NoError;
    mResolveJob = new CollectionResolveJob(mParentCollectionArg);
    connect(mResolveJob, SIGNAL(result(KJob*)), this, SLOT(onParentCollectionFetched(KJob*)));
    mResolveJob->start();
}

void ImportCommand::onParentCollectionFetched(KJob* job)
{
    if (job->error() != 0) {
        emit error(job->errorString());
        emit finished(RuntimeError);
        return;
    }
    CollectionResolveJob *fetchJob = qobject_cast<CollectionResolveJob*>(job);
    mParentCollection = fetchJob->collection();

    if (mMergeCollection) {
	mItems = mXmlDocument->items(mParentCollection, true);
        Q_FOREACH(Item item, mItems) {
            ItemCreateJob *job = new ItemCreateJob(item, mParentCollection);
            connect(job, SIGNAL(result(KJob*)), this, SLOT(onItemCreated(KJob*)));
            job->start();
        }
    } else {
        QMetaObject::invokeMethod(this, "processNextCollection");
    }
}

void ImportCommand::processNextCollection()
{
    if (mCollections.isEmpty()) {
        emit finished(mExitStatus);
        return;
    }

    Q_ASSERT(mParentCollection.isValid() != false);

    Collection c = mCollections.takeFirst();

    c.setParent(mParentCollection);

    CollectionCreateJob *createJob = new CollectionCreateJob(c);
    connect(createJob, SIGNAL(result(KJob*)), this, SLOT(onCollectionCreated(KJob*)));
    createJob->start();
}

void ImportCommand::onCollectionFetched(KJob* job)
{
    CollectionFetchJob *fJob = qobject_cast<CollectionFetchJob*>(job);
    Collection col = fJob->property("collection").value<Collection>();
    if (job->error() != 0) {
        emit error(job->errorString());
        emit error("Creating collection as it does not exist: " + col.name());
        CollectionCreateJob *createJob = new CollectionCreateJob(col);
        connect(createJob, SIGNAL(result(KJob*)), this, SLOT(onCollectionCreated(KJob*)));
        createJob->start();
        return;
    }

    processItems(col);
}

void ImportCommand::processItems(Collection col)
{
    Item::List items = mXmlDocument->items(col);
    Q_FOREACH(Item item, items) {
        ItemCreateJob *job = new ItemCreateJob(item, col);
        connect(job, SIGNAL(result(KJob*)), this, SLOT(onItemCreated(KJob*)));
        job->start();
    }
    QMetaObject::invokeMethod(this, "processNextCollection");
}

void ImportCommand::onCollectionCreated(KJob* job)
{
    if (job->error() != 0) {
        emit error(job->errorString());
        emit finished(RuntimeError);
        return;
    }
    CollectionCreateJob *cJob = qobject_cast<CollectionCreateJob*>(job);
    QMetaObject::invokeMethod(this, "processItems", Qt::QueuedConnection, Q_ARG(Collection, cJob->collection()));
}

void ImportCommand::onItemCreated(KJob* job)
{
    mItems.takeFirst();
    if(mItems.empty())
    {
      emit finished(mExitStatus);
    }
    if (job->error() != 0) {
        emit error(job->errorString());
	mExitStatus = RuntimeError;
        return;
    }
}