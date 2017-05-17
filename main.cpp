#include <QApplication>
#include <QString>
#include <QDebug>
#include "note.h"
#include "noteediteur.h"

int main(int argc, char *argv[])
{
    /*
    try{
    QApplication app(argc, argv);
    QString fichier = QFileDialog::getOpenFileName();
    NotesManager &m=NotesManager::getInstance();
    m.setFilename(fichier);
    m.load();
    Article& a=dynamic_cast<Article&>(m.getNote("id:A2"));
    ArticleEditeur fenetre(a);
    for(NotesManager::Iterator iterator=NotesManager::getInstance().getIterator(); !iterator.isDone(); iterator.next())
    { if (iterator.current().getId()=="id:A2") qDebug()<<"version "<<iterator.current().getVersion()<<" trouvee"<<"\n"; }
    fenetre.show();
    return app.exec();
    }
    catch(NotesException e){qDebug()<<e.getInfo();}
    */

    QApplication app(argc, argv);
    QString fichier = QFileDialog::getOpenFileName();
    NotesManager &m=NotesManager::getInstance();
    m.setFilename(fichier);
    m.load();
    Relation reference("Reference","La note x fait reference a la note y",true);
    Note& note1=m.getNote("id:A1"); //renvoie une référence sur la dernière version de la note
    Note& note2=m.getNote("id:A2");
    reference.addCouple(note1, note2, "label 1");
    reference.addCouple(note2, note1, "label 2");
    reference.set_label_couple(&note1,&note2,"label modifie");
    reference.removeCouple(&note1, &note2);
    QWidget fenetre;
    QLabel* relation_titre= new QLabel(reference.getTitre());
    QLabel* relation_description= new QLabel(reference.getDescription());
    QVBoxLayout *layout=new QVBoxLayout;
    layout-> addWidget(relation_titre);
    layout-> addWidget(relation_description);
    for (unsigned int i=0;i<reference.getNbCouples();i++){
        QLabel* relation_couple= new QLabel(reference.getXCouple(i).getId()+"-->"+reference.getYCouple(i).getId()+" (label : "+reference.getLabelCouple(i)+")");
        layout-> addWidget(relation_couple);
    };
    fenetre.setLayout(layout);
    fenetre.show();
    return app.exec();

}
