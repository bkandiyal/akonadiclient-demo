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

#ifndef CLEARCOMMAND_H
#define CLEARCOMMAND_H

#include <abstractcommand.h>

class CollectionResolveJob;
class KJob;
class ClearCommand : public AbstractCommand
{
    Q_OBJECT

public:
    explicit ClearCommand(QObject* parent = 0);
    virtual ~ClearCommand();
    QString name() const {
        return "clear";
    }

protected:
    virtual int initCommand(KCmdLineArgs* parsedArgs);
    virtual void setupCommandOptions(KCmdLineOptions& options);

public Q_SLOTS:
    void start();

private:
    QString mCollection;
    CollectionResolveJob *mResolveJob;
    
private Q_SLOTS:
    void onCollectionFetched(KJob*);
    void onItemsFetched(KJob*);
    void onItemsDeleted(KJob*);
};

#endif // CLEARCOMMAND_H
