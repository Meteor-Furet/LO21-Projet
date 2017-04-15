#ifndef ARTICLEEDITEUR_H
#define ARTICLEEDITEUR_H
#include <QApplication>
#include <QFileDialog>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDate>
#include <QDateEdit>
#include <note.h>

class NoteEditeur:public QWidget
{
private:
    Q_OBJECT
    Note* note;
    QVBoxLayout* layout;
    QLabel* id1;
    QLabel* titre1;
    QLabel* date1;
    QLabel* date2;
    QLineEdit* id;
    QLineEdit* titre;
    QDateEdit* date_c;
    QDateEdit* date_m;

public:
    explicit NoteEditeur(Note& n, QWidget* parent=0);

};

#endif // ARTICLEEDITEUR_H

