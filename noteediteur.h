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
#include <QMessageBox>
#include <note.h>

class NoteEditeur:public QWidget
{
protected:
    Q_OBJECT
    Note* note;
    QVBoxLayout* layout;
    QLabel* id;
    QLabel* titre1;
    QLineEdit* titre;
    QLabel* date_c;
    QLabel* date_m;
    QLabel* last;
    QLabel* etat;
    QLabel* version;
    QPushButton* bouton;
public:
    explicit NoteEditeur(Note& n, QWidget* parent=0);
    QVBoxLayout* getLayout() {return layout;} //méthode pour récupérer le layout et le modifier en fonction de la sous-classe
    QPushButton* getButton() {return bouton;} //méthode pour récupérer le bouton et le modifier en fonction de la sous-classe
    QLineEdit* getTitle() {return titre;} //méthode pour récupérer le titre et le modifier en fonction de la sous-classe
    virtual void extensionsave()=0;
    Note* getNote() const {return note;}

signals:

public slots:
    void save();
private slots: //à usage interne
    void activerBouton(QString str="");
};


class ArticleEditeur:public NoteEditeur
{
protected:
    Q_OBJECT
    QLabel* text1;
    QTextEdit* text;
public:
    ArticleEditeur(Article& a, QWidget* parent=0);
public slots:
    void extensionsave();

};


#endif // ARTICLEEDITEUR_H

