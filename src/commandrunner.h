/*
    Copyright (C) 2012  Kevin Krammer <krammer@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include "commandfactory.h"

#include <QObject>

class AbstractCommand;

class KAboutData;
class KCmdLineArgs;

class CommandRunner : public QObject
{
  Q_OBJECT

  public:
    CommandRunner( const KAboutData &aboutData, KCmdLineArgs *parsedArgs );
    ~CommandRunner();

    int start();

    static void reportError(const QString &msg);
    static void reportWarning(const QString &msg);
    static void reportFatal(const QString &msg);

  private:
    AbstractCommand *mCommand;
    KCmdLineArgs *mParsedArgs;
    CommandFactory mFactory;
    
  private Q_SLOTS:
    void onCommandFinished( int exitCode );
    void onCommandError( const QString &error );
};

#endif // COMMANDRUNNER_H
