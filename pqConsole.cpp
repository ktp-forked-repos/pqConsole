/*
    pqConsole    : interfacing SWI-Prolog and Qt

    Author       : Carlo Capelli
    E-mail       : cc.carlo.cap@gmail.com
    Copyright (C): 2013, Carlo Capelli

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "pqConsole.h"
#include "ConsoleEdit.h"
#include <QApplication>
#include <QMainWindow>

pqConsole::pqConsole()
{
}

int pqConsole::runDemo(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    w.setCentralWidget(new ConsoleEdit(argc, argv));
    w.show();
    w.setMinimumSize(800, 600);
    return a.exec();
}
