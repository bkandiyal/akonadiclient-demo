#include "exportcommand.h"

#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>

#include <kcmdlineargs.h>
#include <kurl.h>

#include "xml/xmlwriter.h"
#include "xml/xmlwritejob.h"

#include <iostream>
#include <boost/config/posix_features.hpp>

using namespace Akonadi;

ExportCommand::ExportCommand(QObject *parent)
    : AbstractCommand(parent), mResolveJob(0)
{
    mShortHelp = ki18nc("@info:shell", "Export a collection to XML").toString();
}

ExportCommand::~ExportCommand()
{

}

void ExportCommand::setupCommandOptions(KCmdLineOptions &options)
{
    AbstractCommand::setupCommandOptions(options);
    options.add("+collection", ki18nc("@show:shell", "The collection to export"));
    options.add("+file", ki18nc("@show:shell", "The file to save the collection to"));
}

int ExportCommand::initCommand(KCmdLineArgs *parsedArgs)
{
    if (parsedArgs->count() < 2) {
        emitErrorSeeHelp(ki18nc("@show:shell", "No collection specified"));
        return InvalidUsage;
    } else if (parsedArgs->count() < 3) {
        emitErrorSeeHelp(ki18nc("@show:shell", "Please specify a file to save the collection to"));
        return InvalidUsage;
    }

    mCollection = parsedArgs->arg(1);
    mExportPath = parsedArgs->arg(2);

    mResolveJob = new CollectionResolveJob(mCollection, this);
    if (!mResolveJob->hasUsableInput()) {
        emit error(mResolveJob->errorString());
        delete mResolveJob;
        mResolveJob = 0;

        return InvalidUsage;
    }
    return NoError;
}

void ExportCommand::start()
{
    Q_ASSERT(mResolveJob != 0);
    connect(mResolveJob, SIGNAL(result(KJob*)), this, SLOT(onCollectionFetched(KJob*)));
    mResolveJob->start();
}

void ExportCommand::onCollectionFetched(KJob *job)
{
    std::cout << "Collection fetched\n";
    if (job->error() != 0) {
        emit error(job->errorString());
        mExitStatus = RuntimeError;
    } else {
        XmlWriteJob *writeJob = new XmlWriteJob(mResolveJob->collection(), mExportPath, this);

        connect(writeJob, SIGNAL(result(KJob*)), SLOT(onWriteFinished(KJob*)));
        writeJob->start();
    }
}

void ExportCommand::onWriteFinished(KJob *job)
{
    if (job->error() != 0) {
        emit error(job->errorString());
        mExitStatus = RuntimeError;
    } else {
        mExitStatus = NoError;
        emit finished(mExitStatus);
    }
}
