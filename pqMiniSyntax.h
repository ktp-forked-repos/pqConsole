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

#ifndef PQMINISYNTAX_H
#define PQMINISYNTAX_H

#include "pqConsole_global.h"
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QElapsedTimer>
#include <QPlainTextEdit>

/** a minimal Prolog syntax highlighter
 */
class PQCONSOLESHARED_EXPORT pqMiniSyntax : public QSyntaxHighlighter
{
    Q_OBJECT
public:

    pqMiniSyntax(QObject *parent = 0) : QSyntaxHighlighter(parent) { setup(); }
    pqMiniSyntax(QTextDocument *parent) : QSyntaxHighlighter(parent) { setup(); }
    pqMiniSyntax(QTextEdit *parent)  : QSyntaxHighlighter(parent) { setup(); }
    pqMiniSyntax(QPlainTextEdit *parent)  : QSyntaxHighlighter(parent) { setup(); }

signals:

public slots:

protected:

    // handle state tracking using currentBlockState/previousBlockState
    virtual void highlightBlock(const QString &text);

private:

    enum token_name {
        Comment,
        Atom,
        Atomq,
        Atombackq,
        String,
        Variable,
        Number,
        Operator,
        CharCode,
        Unknown
    };
    QTextCharFormat fmt[Unknown+1];
    QRegExp tokens;

    void setup();
    QElapsedTimer startToEnd;
};

#endif // PLMINISYNTAX_H
