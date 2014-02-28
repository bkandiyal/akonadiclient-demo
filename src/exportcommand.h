#ifndef EXPORTCOMMAND_H
#define EXPORTCOMMAND_H

#include "abstractcommand.h"
#include "collectionresolvejob.h"

class KJob;

class ExportCommand : public AbstractCommand
{
    Q_OBJECT
public:
    explicit ExportCommand(QObject *parent = 0);
    ~ExportCommand();

    QString name() const {
        return QLatin1String("export");
    }

public Q_SLOTS:
    void start();

protected:
    void setupCommandOptions(KCmdLineOptions &options);
    int initCommand(KCmdLineArgs *parsedArgs);

private:
    QString mCollection;
    QString mExportPath;
    AbstractCommand::Errors mExitStatus;
    CollectionResolveJob *mResolveJob;

private Q_SLOTS:
    void onWriteFinished(KJob *job);
    void onCollectionFetched(KJob *job);
};

#endif // EXPORTCOMMAND_H
