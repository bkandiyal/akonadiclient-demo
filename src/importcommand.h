#ifndef IMPORTCOMMAND_H
#define IMPORTCOMMAND_H

#include "abstractcommand.h"
#include "xml/xmldocument.h"

#include <akonadi/collection.h>
#include <akonadi/item.h>

class CollectionResolveJob;
class KJob;

class ImportCommand : public AbstractCommand
{
    Q_OBJECT
public:
    ImportCommand(QObject *parent = 0);
    ~ImportCommand();

    QString name() const {
        return QLatin1String("import");
    }

public Q_SLOTS:
    void start();

protected:
    void setupCommandOptions(KCmdLineOptions &options);
    int initCommand(KCmdLineArgs *parsedArgs);

private:
    QString mImportFile;
    QString mParentCollectionArg;
    bool mMergeCollection;
    QString mCollectionArg;
    Akonadi::Item::List mItems;
    Akonadi::Collection mParentCollection;
    Akonadi::Collection mCollection;
    Akonadi::Collection::List mCollections;
    AbstractCommand::Errors mExitStatus;
    Akonadi::XmlDocument *mXmlDocument;
    CollectionResolveJob *mResolveJob;

private Q_SLOTS:
    void onParentCollectionFetched(KJob *job);
    void onCollectionFetched(KJob *job);
    void onCollectionCreated(KJob *job);
    void onItemCreated(KJob *job);
    void processNextCollection();
    void processItems(Akonadi::Collection col);
};

#endif // IMPORTCOMMAND_H
