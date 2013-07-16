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

#include "Swipl_IO.h"
#include "PREDICATE.h"
#include <QDebug>

Swipl_IO::Swipl_IO(QObject *parent) :
    QObject(parent), host(0)
{
}

/** fill the buffer */
ssize_t Swipl_IO::_read_f(void *handle, char *buf, size_t bufsize) {
    //qDebug() << "Swipl_IO::_read_f" << CVP(handle) << CVP(CT);
    auto e = static_cast<Swipl_IO*>(handle);
    return e->_read_(buf, bufsize);
}

/** empty the buffer */
ssize_t Swipl_IO::_write_f(void *handle, char* buf, size_t bufsize) {
    //qDebug() << "Swipl_IO::_write_f" << CVP(handle) << CVP(CT) << QString::fromUtf8(buf, bufsize);
    auto e = static_cast<Swipl_IO*>(handle);
    emit e->user_output(QString::fromUtf8(buf, bufsize));
    return bufsize;
}

/** seek to position */
long Swipl_IO::_seek_f(void *handle, long pos, int whence) {
    Q_UNUSED(handle);
    Q_UNUSED(pos);
    Q_UNUSED(whence);
    //qDebug() << "Swipl_IO::_seek_f" << CVP(handle) << CVP(CT);
    return 0;
}

/** close stream */
int Swipl_IO::_close_f(void *handle) {
    Q_UNUSED(handle);
    //qDebug() << "_close_f" << CVP(handle) << CVP(CT);
    return 0;
}

/** Info/control */
int Swipl_IO::_control_f(void *handle, int action, void *arg) {
    Q_UNUSED(handle);
    Q_UNUSED(action);
    Q_UNUSED(arg);
    //qDebug() << "Swipl_IO::_control_f" << CVP(handle) << action;
    return -1;
}

/** seek to position (big file) */
int64_t Swipl_IO::_seek64_f(void *handle, int64_t pos, int whence) {
    Q_UNUSED(handle);
    Q_UNUSED(pos);
    Q_UNUSED(whence);
    //qDebug() << "_seek64_f" << CVP(handle) << CVP(CT);
    return 0;
}

ssize_t Swipl_IO::_read_(char *buf, size_t bufsize) {

    //qDebug() << "Swipl_IO::_read_" << CVP(this) << CVP(CT);
    PL_write_prompt(TRUE);

    emit user_prompt(PL_thread_self());
    ready.wait(&sync);

    //qDebug() << "Swipl_IO::AWAKE" << CVP(this) << CVP(CT);

    uint n = buffer.length();
    Q_ASSERT(bufsize >= n);
    uint l = bufsize < n ? bufsize : n;
    qstrncpy(buf, buffer, l + 1);
    return l;
}

void Swipl_IO::user_input(QString s) {
    //qDebug() << "Swipl_IO::user_input available" << CVP(this) << CVP(CT);
    buffer = s.toUtf8();
    ready.wakeOne();
}

void Swipl_IO::wait_console() {
    //qDebug() << "Swipl_IO::wait_console" << CVP(this) << CVP(CT);
    sync.lock();
    //bool rc =
    ready.wait(&sync);
    //qDebug() << "rc attached()" << CVP(CT) << rc;
}

void Swipl_IO::take_input(QString cmd) {
    //qDebug() << "Swipl_IO::take_input" << CVP(this) << CVP(CT) << cmd;
    buffer = cmd.toUtf8();
    ready.wakeOne();
}

void Swipl_IO::attached(ConsoleEdit *c) {
    Q_ASSERT(host == c);
    ready.wakeOne();
}