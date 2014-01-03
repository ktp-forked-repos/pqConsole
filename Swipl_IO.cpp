/*
    pqConsole    : interfacing SWI-Prolog and Qt

    Author       : Carlo Capelli
    E-mail       : cc.carlo.cap@gmail.com
    Copyright (C): 2013,2014 Carlo Capelli

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
#include "pqMainWindow.h"
#include <QDebug>
#include <QTime>

Swipl_IO::Swipl_IO(QObject *parent) :
    QObject(parent)
{
}

/** fill the buffer */
ssize_t Swipl_IO::_read_f(void *handle, char *buf, size_t bufsize) {
    auto e = pq_cast<Swipl_IO>(handle);
    return e->_read_(buf, bufsize);
}

/** empty the buffer */
ssize_t Swipl_IO::_write_f(void *handle, char* buf, size_t bufsize) {
    auto e = pq_cast<Swipl_IO>(handle);
    if (e->target) {
        emit e->user_output(QString::fromUtf8(buf, bufsize));
        e->flush();
    }
    return bufsize;
}

/** seek to position */
long Swipl_IO::_seek_f(void *handle, long pos, int whence) {
    Q_UNUSED(handle);
    Q_UNUSED(pos);
    Q_UNUSED(whence);
    return 0;
}

/** close stream */
int Swipl_IO::_close_f(void *handle) {
    Q_UNUSED(handle);
    return 0;
}

/** Info/control */
int Swipl_IO::_control_f(void *handle, int action, void *arg) {
    Q_UNUSED(handle);
    Q_UNUSED(action);
    Q_UNUSED(arg);
    return -1;
}

/** seek to position (big file) */
int64_t Swipl_IO::_seek64_f(void *handle, int64_t pos, int whence) {
    Q_UNUSED(handle);
    Q_UNUSED(pos);
    Q_UNUSED(whence);
    return 0;
}

/** polling loop til buffer ready
 */
ssize_t Swipl_IO::_read_(char *buf, size_t bufsize) {

    qDebug() << "_read_" << CVP(target);
    int thid = PL_thread_self();

    // handle setup interthread and termination
    for ( ; ; ) {
        {   QMutexLocker lk(&sync);
            if (target) {
                if (!target->thids.contains(thid)) {
                    target->add_thread(thid);
                    int rc =
                    PL_thread_at_exit(eng_at_exit, this, FALSE);
                    qDebug() << "installed" << rc;
                }
                break;
            }
        }

	if ( PL_handle_signals() < 0 )
	    return -1;

        SwiPrologEngine::msleep(10);
    }

    if ( buffer.isEmpty() ) {
        PL_write_prompt(TRUE);
	emit user_prompt(thid, SwiPrologEngine::is_tty(this));
    }

    for ( ; ; ) {

        {   QMutexLocker lk(&sync);

            if (!query.isEmpty()) {
                try {
                    int rc = PlCall(query.toStdWString().data());
                    qDebug() << "PlCall" << query << rc;
                }
                catch(PlException e) {
                    qDebug() << t2w(e);
                }
                query.clear();
            }

            uint n = buffer.length();
            Q_ASSERT(bufsize >= n);
            if (n > 0) {
                uint l = bufsize < n ? bufsize : n;
                memcpy(buf, buffer, l);
                buffer.remove(0, l);
                return l;
            }

            if (target->status == ConsoleEdit::eof) {
	        target->status = ConsoleEdit::running;
                return 0;
	    }
        }

	if ( PL_handle_signals() < 0 )
	    return -1;

        SwiPrologEngine::msleep(10);
    }
}

/** syncronized storage of user input from console front end
 */
void Swipl_IO::user_input(QString s) {
    QMutexLocker lk(&sync);
    buffer = s.toUtf8();
}

void Swipl_IO::take_input(QString cmd) {
    QMutexLocker lk(&sync);
    buffer = cmd.toUtf8();
}

void Swipl_IO::eng_at_exit(void *p) {
    auto e = pq_cast<Swipl_IO>(p);
    emit e->sig_eng_at_exit();
}

void Swipl_IO::attached(ConsoleEdit *c) {
    QMutexLocker lk(&sync);
    Q_ASSERT(target == 0);
    target = c;
}

void Swipl_IO::query_run(QString newquery) {
    QMutexLocker lk(&sync);
    Q_ASSERT(query.isEmpty());
    query = newquery;
}
