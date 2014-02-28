/*
 * Copyright 2014  Bhaskar Kandiyal <bkandiyal@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "clearcommand.h"
#include "collectionresolvejob.h"

#include <akonadi/itemfetchjob.h>
#include <akonadi/itemdeletejob.h>

#include <kcmdlineargs.h>

using namespace Akonadi;

ClearCommand::ClearCommand(QObject *parent)
    : AbstractCommand(parent)
{
    mShortHelp = ki18nc("@info:shell", "Delete all items in a collection").toString();
}

ClearCommand::~ClearCommand()
{

}

void ClearCommand::setupCommandOptions(KCmdLineOptions& options)
{
    AbstractCommand::setupCommandOptions(options);
    options.add("+collection", ki18nc("@info:shell", "Collection to clear"));
}

int ClearCommand::initCommand(KCmdLineArgs* parsedArgs)
{
    if (parsedArgs->count() < 2) {
        emitErrorSeeHelp(ki18nc("@info:shell", "Please specify a collection to clear"));
        return InvalidUsage;
    }

    mCollection = parsedArgs->arg(1);

    return NoError;
}

void ClearCommand::start()
{
    CollectionResolveJob *job = new CollectionResolveJob(mCollection, this);
    connect(job, SIGNAL(result(KJob*)), SLOT(onCollectionFetched(KJob*)));
    job->start();
}

void ClearCommand::onCollectionFetched(KJob* job)
{
    CollectionResolveJob *resJob = qobject_cast<CollectionResolveJob*>(job);
    Collection c = resJob->collection();
    ItemFetchJob *fJob = new ItemFetchJob(c, this);
    connect(fJob, SIGNAL(result(KJob*)), SLOT(onItemsFetched(KJob*)));
    fJob->start();
}

void ClearCommand::onItemsFetched(KJob* job)
{
    if (job->error() != 0) {
        emit error(job->errorString());
        emit finished(RuntimeError);
        return;
    }

    ItemFetchJob *fetchJob = qobject_cast<ItemFetchJob*>(job);
    Item::List items = fetchJob->items();
    ItemDeleteJob *delJob = new ItemDeleteJob(items, this);
    connect(delJob, SIGNAL(result(KJob*)), SLOT(onItemsDeleted(KJob*)));
    delJob->start();
}

void ClearCommand::onItemsDeleted(KJob *job)
{
    if (job->error() != 0) {
        emit error(job->errorString());
        emit finished(RuntimeError);
    } else {
        emit finished(NoError);
    }
}
