#include "note.h"
#include "relation.h"
#include <QFile>
#include <QTextCodec>
#include <QtXml>
#include <QMessageBox>
#include <typeinfo>

/**
 * \file      note.cpp
 * \brief     Définit les méthodes relatives aux classes note, ses dérivées et NotesManager.
 */

/*======================================================= Note et classes dérivées ====================================================================================*/
Note::~Note(){}
Article::~Article(){}
Tache::~Tache(){}
TacheAvecPriorite::~TacheAvecPriorite(){}
TacheAvecDeadline::~TacheAvecDeadline(){}
Fichier::~Fichier(){}

/*============================================================= NotesManager ==========================================================================================*/
//destructeur
NotesManager::~NotesManager(){
    nettoyer_archives();
    if (vidage_corbeille) viderCorbeille();
    if (filename!="") save();
    delete[] notes;
}

//singleton
NotesManager& NotesManager::getInstance(){
    static NotesManager instance;
    return instance;
}

//permet d'ajouter une note dans le tableau de Notes du NotesManager
void NotesManager::addNote(Note* n)
{
    if (n->getVersion()==1) //il s'agit d'une nouvelle note, on vérifie que l'id n'est pas déjà utilisé
        for(unsigned int i=0; i<nbNotes; i++)
            if (notes[i]->getId()==n->getId()) throw NotesException("Erreur, cet identificateur est deja utilise");
    else //il s'agit d'une nouvelle version d'une note, on vérifie que toutes les versions antérieures existent
        for(unsigned int i=1; i<n->getVersion(); i++)
            Note& v_i=getVersionNote(n->getId(),i); //si une version n'existe pas, une exception est déclenchée dans getVersionNote

    if (nbNotes==nbMaxNotes){
        //le tableau de note nécessite un agrandissement
        Note** newNotes= new Note*[nbMaxNotes+5];
        for(unsigned int i=0; i<nbNotes; i++) newNotes[i]=notes[i];
        Note** oldNotes=notes;
        notes=newNotes;
        nbMaxNotes+=5;
        if (oldNotes) delete[] oldNotes;
    }
    notes[nbNotes++]=n;
    if (!(isLoading)) {
        RelationsManager::Iterator iterator_manager=RelationsManager::getInstance().getIterator();
        iterator_manager.debut();
        while(!iterator_manager.isDone()){ //ici on examine chaque relation
           Relation::Iterator iterator_relation= iterator_manager.current().getIterator();
           iterator_relation.debut();
           if (iterator_manager.current().getTitre()=="Reference") //alors on enlève tous les couples tels que noteX.id=n.id
           {
                while(!iterator_relation.isDone()) //ici on examine chaque couple de la relation
                {
                    if (iterator_relation.current_noteX().getId()==n->getId()) iterator_manager.current().removeCouple(iterator_relation.current_noteX(),iterator_relation.current_noteY());
                    iterator_relation.next();
                }
                check_reference(*n);
           }
           else
           {
               while(!iterator_relation.isDone()) //ici on examine chaque couple de la relation
               {
                   if (iterator_relation.current_noteX().getId()==n->getId())
                   {
                       Note& y=iterator_relation.current_noteY();
                       QString label=iterator_relation.current_label();
                       iterator_manager.current().removeCouple(iterator_relation.current_noteX(),y);
                       iterator_manager.current().addCouple(*n,y,label);
                   }
                   if (iterator_relation.current_noteY().getId()==n->getId())
                   {
                       Note& x=iterator_relation.current_noteX();
                       QString label=iterator_relation.current_label();
                       iterator_manager.current().removeCouple(x,iterator_relation.current_noteY());
                       iterator_manager.current().addCouple(x,*n,label);
                   }
                   iterator_relation.next();
               }
           }
           iterator_manager.next();
    }
}
}

void NotesManager::check_reference(Note& n)
{
    if (n.getEtat()==active)
    {
    search_reference(n, n.getTitre());
    if (typeid(n)==typeid(Article))
    {   Article& a=dynamic_cast<Article&>(n); search_reference(a, a.getTexte());
    }
    if (typeid(n)==typeid(Tache))
    {   Tache& t=dynamic_cast<Tache&>(n); search_reference(t, t.getTexte());
    }
    if (typeid(n)==typeid(Fichier))
    {   Fichier& f=dynamic_cast<Fichier&>(n); search_reference(f, f.getDescription());
    }
    }
}

void NotesManager::search_reference(Note& n, const QString& texte)
{
    RelationsManager &manager_relations=RelationsManager::getInstance();
    if (texte.contains("ref{"))
    {
        for (unsigned int i=0; i<nbNotes ; i++)
        {
            if (texte.contains("ref{"+notes[i]->getId()+"}")) manager_relations.getRelation("Reference").addCouple(n,*notes[i],"");
        }
    }
}

//permet de renvoyer la dernière version d'une note via le NotesManager
Note& NotesManager::getNote(const QString& id)
{
    // si la note existe déjà, on en renvoie une référence
    for(unsigned int i=0; i<nbNotes; i++)
        if (notes[i]->getId()==id && notes[i]->IsLast()) return *notes[i];
    throw NotesException("Note inexistante");
}

//permet de renvoyer une version particulière d'une note via le NotesManager
Note& NotesManager::getVersionNote(const QString& id, unsigned int v)
{
    for(unsigned int i=0; i<nbNotes; i++)
        if (notes[i]->getId()==id && notes[i]->getVersion()==v) return *notes[i];
    throw NotesException("Note ou version inexistante");
}

//permet de créer un article et l'ajouter au NotesManager
void NotesManager::addArticle(const QString& id, const QString& ti, const QString& te, const QDate date_c, const QDate date_m, const unsigned int v, bool last, const NoteEtat etat)
{
    Article* a=new Article(id,ti,te,date_c,date_m,v,last,etat);
    addNote(a);
}

//permet de créer une tâche et l'ajouter au NotesManager
void NotesManager::addTache(const QString& id, const QString& ti, const QString& te, const QDate date_c, const QDate date_m, unsigned int v, bool last, NoteEtat etat, TacheStatut st)
{
    Tache* t=new Tache(id,ti,te,date_c,date_m,last,v,etat,st);
    addNote(t);
}

//permet de créer une tâche avec priorité et l'ajouter au NotesManager
void NotesManager::addTacheAvecPriorite(const QString& id, const QString& ti, const QString& te, const QDate date_c, const QDate date_m, unsigned int v, bool last, NoteEtat etat, TacheStatut st,unsigned int priorite)
{
    TacheAvecPriorite* tp=new TacheAvecPriorite(id,ti,te,date_c,date_m,last,v,etat,st,priorite);
    addNote(tp);
}

//permet de créer une tâche avec deadline et l'ajouter au NotesManager
void NotesManager::addTacheAvecDeadline(const QString& id, const QString& ti, const QString& te, const QDate date_c, const QDate date_m, unsigned int v, bool last, NoteEtat etat, TacheStatut st, const QDate deadline)
{
    TacheAvecDeadline* td=new TacheAvecDeadline(id,ti,te,deadline,date_c,date_m,last,v,etat,st);
    addNote(td);
}

//permet de créer un fichier et l'ajouter au NotesManager
void NotesManager::addFichier(const QString& id, const QString& ti, const QString& descr, const QDate date_c, const QDate date_m, unsigned int v, bool last, NoteEtat etat, const QString& filename, FichierType ty)
{
    Fichier* f=new Fichier(id,ti,ty,descr,filename,date_c, date_m, last, v, etat);
    addNote(f);
}

//permet de supprimer une note
void NotesManager::deleteNote(Note& n)
{
    //on regarde d'abord si la note est en relation via Reference
    RelationsManager &manager_relations=RelationsManager::getInstance();
    Relation& reference=manager_relations.getRelation("Reference");
    unsigned int i=0;
    while(i<reference.getNbCouples() && reference.getYCouple(i).getId()!=n.getId())
        i++;
    if(i<reference.getNbCouples()) //la note apparait dans la relation reference -> on ne peut pas la supprimer -> on l'archive
       { n.setEtat(archivee);
         for(unsigned int v=1; v<n.getVersion(); v++) getVersionNote(n.getId(),v).setEtat(archivee);
    }
    else { //la note n'apparait pas dans la relation reference -> on peut la supprimer -> mise à la corbeille et suppression des couples où n est impliquée
        n.setEtat(corbeille);
        for(unsigned int v=1; v<n.getVersion(); v++) getVersionNote(n.getId(),v).setEtat(corbeille);
        for(unsigned int i=0; i<manager_relations.getNbRelations(); i++)
            for(unsigned int j=0; j<manager_relations.getIRelation(i).getNbCouples();j++)
                if (manager_relations.getIRelation(i).getXCouple(j).getId()==n.getId() || manager_relations.getIRelation(i).getYCouple(j).getId()==n.getId())
                    manager_relations.getIRelation(i).removeCouple(manager_relations.getIRelation(i).getXCouple(j),manager_relations.getIRelation(i).getYCouple(j));
    }
}

//méthode pour rendre active une note qui avait été archivée
void NotesManager::restoreNote(Note& n)
{
    for(unsigned int v=1; v<=n.getVersion(); v++) getVersionNote(n.getId(),v).setEtat(active);
}

//méthode pour vider la corbeille (supprimer définitivement les notes)
void NotesManager::viderCorbeille()
{
    for(unsigned int i=0;i<nbNotes;i++)
        if(notes[i]->getEtat()==corbeille)
        { delete notes[i];
          for(unsigned int j=i;j<nbNotes;j++)
              notes[j]=notes[j+1];
          nbNotes--;
        }
}

//méthode pour nettoyer les archives
void NotesManager::nettoyer_archives()
{
    RelationsManager &manager_relations=RelationsManager::getInstance();
    Relation& reference=manager_relations.getRelation("Reference");
    for (unsigned int k=0; k<nbNotes; k++)
        if (notes[k]->getEtat()==archivee && notes[k]->IsLast())
        {
            unsigned int i=0;
            while(i<reference.getNbCouples() && reference.getYCouple(i).getId()!=notes[k]->getId())
                i++;
            if(i>=reference.getNbCouples()) //la note n'apparait plus dans la relation reference -> on la place à la corbeille
               {
                 notes[k]->setEtat(corbeille);
                 for(unsigned int v=1; v<notes[k]->getVersion(); v++) getVersionNote(notes[k]->getId(),v).setEtat(corbeille);
               }
        }
}

//permet de charger un fichier de Notes
void NotesManager::load() {
    isLoading=true;
    QFile fin(filename);
    // If we can't open it, let's show an error message.
    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw NotesException("Erreur ouverture fichier notes");
    }
    // QXmlStreamReader takes any QIODevice.
    QXmlStreamReader xml(&fin);
    // We'll parse the XML until we reach end of it.
    while(!xml.atEnd() && !xml.hasError()) {
        // Read next element.
        QXmlStreamReader::TokenType token = xml.readNext();
        // If token is just StartDocument, we'll go to next.
        if(token == QXmlStreamReader::StartDocument) continue;
        // If token is StartElement, we'll see if we can read it.
        if(token == QXmlStreamReader::StartElement) {
            if(xml.name() == "notes") continue;
                if(xml.name() == "article") {
                    QString identificateur;
                    QString titre;
                    QString text;
                    unsigned int version;
                    QDate date_c;
                    QDate date_m;
                    NoteEtat etat;
                    bool isLast;
                    QXmlStreamAttributes attributes = xml.attributes();
                    xml.readNext();
                    //We're going to loop over the things because the order might change.
                    //We'll continue the loop until we hit an EndElement named article.
                    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "article")) {
                        if(xml.tokenType() == QXmlStreamReader::StartElement) {
                            //identificateur
                            if(xml.name() == "id") {
                                xml.readNext();
                                identificateur=xml.text().toString();}
                            //titre
                            if(xml.name() == "title") {
                                xml.readNext();
                                titre=xml.text().toString();}
                            //text
                            if(xml.name() == "text") {
                                xml.readNext();
                                text=xml.text().toString();}
                            //version
                            if(xml.name() == "version") {
                                xml.readNext();
                                QString version_lue=xml.text().toString();
                                bool conversion;
                                version = version_lue.toUInt(&conversion, 10);
                                if (conversion==false) throw NotesException("Erreur dans conversion QString to Int (version)");}
                            //date de creation
                            if(xml.name() == "creation") {
                                xml.readNext();
                                QString date_creation_lue=xml.text().toString();
                                date_c = QDate::fromString(date_creation_lue,"dd/MM/yyyy");}
                            //date de modification
                            if(xml.name() == "modification") {
                                xml.readNext();
                                QString date_modification_lue=xml.text().toString();
                                date_m = QDate::fromString(date_modification_lue,"dd/MM/yyyy");}
                            //etat
                            if(xml.name() == "etat") {
                                xml.readNext();
                                QString etat_lu=xml.text().toString();
                                if (latinCompare(etat_lu, "Active")) etat=active;
                                else {if (latinCompare(etat_lu, "Archivee")) etat=archivee;
                                      else {if (latinCompare(etat_lu, "Corbeille")) etat=corbeille;
                                            else throw NotesException("Erreur dans lecture etat xml");}}}
                            //last
                            if(xml.name() == "last") {
                                xml.readNext();
                                QString last_lu=xml.text().toString();
                                if (latinCompare(last_lu, "oui")) isLast=true;
                                else {if (latinCompare(last_lu, "non")) isLast=false;}}
                    }
                    // ...and next...
                    xml.readNext();
                }
                addArticle(identificateur,titre,text,date_c,date_m,version,isLast,etat);
            }
            else
            if(xml.name() == "tache") {
                QString identificateur;
                QString titre;
                QString text;
                unsigned int version;
                bool p_trouvee=false;
                bool d_trouvee=false;
                unsigned int priorite;
                QDate deadline;
                QDate date_c;
                QDate date_m;
                NoteEtat etat;
                TacheStatut statut;
                bool isLast;
                QXmlStreamAttributes attributes = xml.attributes();
                xml.readNext();
                //We'll continue the loop until we hit an EndElement named article.
                while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "tache")) {
                    if(xml.tokenType() == QXmlStreamReader::StartElement) {
                        //identificateur
                        if(xml.name() == "id") {
                            xml.readNext();
                            identificateur=xml.text().toString();}
                        //titre
                        if(xml.name() == "title") {
                            xml.readNext();
                            titre=xml.text().toString();}
                        //text
                        if(xml.name() == "text") {
                            xml.readNext();
                            text=xml.text().toString();}
                        //version
                        if(xml.name() == "version") {
                            xml.readNext();
                            QString version_lue=xml.text().toString();
                            bool conversion;
                            version = version_lue.toUInt(&conversion, 10);
                            if (conversion==false) throw NotesException("Erreur dans conversion QString to Int (version)");}
                        //date de creation
                        if(xml.name() == "creation") {
                            xml.readNext();
                            QString date_creation_lue=xml.text().toString();
                            date_c = QDate::fromString(date_creation_lue,"dd/MM/yyyy");}
                        //date de modification
                        if(xml.name() == "modification") {
                            xml.readNext();
                            QString date_modification_lue=xml.text().toString();
                            date_m = QDate::fromString(date_modification_lue,"dd/MM/yyyy");}
                        //etat
                        if(xml.name() == "etat") {
                            xml.readNext();
                            QString etat_lu=xml.text().toString();
                            if (latinCompare(etat_lu, "Active")) etat=active;
                            else {if (latinCompare(etat_lu, "Archivee")) etat=archivee;
                                  else {if (latinCompare(etat_lu, "Corbeille")) etat=corbeille;
                                        else throw NotesException("Erreur dans lecture etat xml");}}}
                        //last
                        if(xml.name() == "last") {
                            xml.readNext();
                            QString last_lu=xml.text().toString();
                            if (latinCompare(last_lu, "oui")) isLast=true;
                            else {if (latinCompare(last_lu, "non")) isLast=false;}}
                        //statut
                        if(xml.name() == "statut") {
                            xml.readNext();
                            QString statut_lu=xml.text().toString();
                            if (latinCompare(statut_lu, "Attente")) statut=attente;
                            else {if (latinCompare(statut_lu, "Cours")) statut=cours;
                                  else {if (latinCompare(statut_lu, "Terminee")) statut=terminee;
                                        else throw NotesException("Erreur dans lecture statut (tache) xml");}}}
                        //priorite
                        if(xml.name() == "priorite") {
                            p_trouvee=true;
                            xml.readNext();
                            QString priorite_lue=xml.text().toString();
                            bool conversion;
                            priorite = priorite_lue.toUInt(&conversion, 10);
                            if (conversion==false) throw NotesException("Erreur dans conversion QString to Int (priorite)");}
                        //deadline
                        if(xml.name() == "deadline") {
                            d_trouvee=true;
                            xml.readNext();
                            QString deadline_lue=xml.text().toString();
                            deadline = QDate::fromString(deadline_lue,"dd/MM/yyyy");}
                    }
                    // ...and next...
                    xml.readNext();
                }
                if (!(p_trouvee || d_trouvee)) addTache(identificateur,titre,text,date_c,date_m,version,isLast,etat,statut);
                else if (p_trouvee && !(d_trouvee)) addTacheAvecPriorite(identificateur,titre,text,date_c,date_m,version,isLast,etat,statut,priorite);
                     else if (d_trouvee && !(p_trouvee)) addTacheAvecDeadline(identificateur,titre,text,date_c,date_m,version,isLast,etat,statut,deadline);
            }
            else
            if(xml.name() == "fichier") {
                    QString identificateur;
                    QString titre;
                    QString description;
                    QString filename_fichier;
                    unsigned int version;
                    QDate date_c;
                    QDate date_m;
                    NoteEtat etat;
                    FichierType type;
                    bool isLast;
                    QXmlStreamAttributes attributes = xml.attributes();
                    xml.readNext();
                    //We'll continue the loop until we hit an EndElement named article.
                    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "fichier")) {
                        if(xml.tokenType() == QXmlStreamReader::StartElement) {
                            //identificateur
                            if(xml.name() == "id") {
                                xml.readNext();
                                identificateur=xml.text().toString();}
                            //titre
                            if(xml.name() == "title") {
                                xml.readNext();
                                titre=xml.text().toString();}
                            //description
                            if(xml.name() == "description") {
                                xml.readNext();
                                description=xml.text().toString();}
                            //version
                            if(xml.name() == "version") {
                                xml.readNext();
                                QString version_lue=xml.text().toString();
                                bool conversion;
                                version = version_lue.toUInt(&conversion, 10);
                                if (conversion==false) throw NotesException("Erreur dans conversion QString to Int (version)");}
                            //date de creation
                            if(xml.name() == "creation") {
                                xml.readNext();
                                QString date_creation_lue=xml.text().toString();
                                date_c = QDate::fromString(date_creation_lue,"dd/MM/yyyy");}
                            //date de modification
                            if(xml.name() == "modification") {
                                xml.readNext();
                                QString date_modification_lue=xml.text().toString();
                                date_m = QDate::fromString(date_modification_lue,"dd/MM/yyyy");}
                            //etat
                            if(xml.name() == "etat") {
                                xml.readNext();
                                QString etat_lu=xml.text().toString();
                                if (latinCompare(etat_lu, "Active")) etat=active;
                                else {if (latinCompare(etat_lu, "Archivee")) etat=archivee;
                                      else {if (latinCompare(etat_lu, "Corbeille")) etat=corbeille;
                                            else throw NotesException("Erreur dans lecture etat xml");}}}
                            //last
                            if(xml.name() == "last") {
                                xml.readNext();
                                QString last_lu=xml.text().toString();
                                if (latinCompare(last_lu, "oui")) {isLast=true; qDebug()<<"last true";}
                                else {if (latinCompare(last_lu, "non")) {isLast=false; qDebug()<<"last false";}}}
                            //type
                            if(xml.name() == "type") {
                                xml.readNext();
                                QString type_lu=xml.text().toString();
                                if (latinCompare(type_lu, "Image")) type=image;
                                else  if (latinCompare(type_lu, "Audio")) type=audio;
                                      else  if (latinCompare(type_lu, "Video")) type=video;
                                            else throw NotesException("Erreur dans lecture type (fichier) xml");}
                            //filename
                            if(xml.name() == "filename") {
                                xml.readNext();
                                filename_fichier=xml.text().toString();;}
                        }
                        // ...and next...
                        xml.readNext();
                    }
            }
        }
    }
    // Error handling.
if(xml.hasError()) throw NotesException("Erreur lecteur fichier notes, parser xml");
// Removes any device() or data from the reader * and resets its internal state to the initial state.=
xml.clear();
isLoading=false;
//important que toutes les notes aient été crées en mémoire afin de détecter les relations
for(unsigned int i=0; i<nbNotes; i++)
    if (notes[i]->IsLast()) check_reference(*notes[i]);
}

//permet de sauvegarder les notes dans le fichier
void NotesManager::save() const {
    QFile newfile(filename);
    if (!newfile.open(QIODevice::WriteOnly | QIODevice::Text))
        throw NotesException(QString("Erreur dans la sauvegarde : echec lors de l'ouverture du fichier xml de notes"));
    QXmlStreamWriter stream(&newfile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("notes");
    for(unsigned int i=0; i<nbNotes; i++){
        if (typeid(*notes[i])==typeid(Article))
        {
            Article& n=dynamic_cast<Article&>(*notes[i]);
            stream.writeStartElement("article");
            stream.writeTextElement("text",n.getTexte());
        }
        else if (typeid(*notes[i])==typeid(Tache))
        {
            Tache& n=dynamic_cast<Tache&>(*notes[i]); stream.writeStartElement("tache");
            stream.writeTextElement("text",n.getTexte());
            if (n.getStatut()==attente) stream.writeTextElement("statut","Attente");
            else if (n.getStatut()==cours) stream.writeTextElement("statut","Cours");
            else stream.writeTextElement("statut","Terminee");
        }
        else if (typeid(*notes[i])==typeid(TacheAvecPriorite))
        {
            TacheAvecPriorite& n=dynamic_cast<TacheAvecPriorite&>(*notes[i]); stream.writeStartElement("tache");
            stream.writeTextElement("text",n.getTexte());
            if (n.getStatut()==attente) stream.writeTextElement("statut","Attente");
            else if (n.getStatut()==cours) stream.writeTextElement("statut","Cours");
            else stream.writeTextElement("statut","Terminee");
            stream.writeTextElement("priorite",QString::number(n.getPriorite()));
        }
        else if (typeid(*notes[i])==typeid(TacheAvecDeadline))
        {
            TacheAvecDeadline& n=dynamic_cast<TacheAvecDeadline&>(*notes[i]); stream.writeStartElement("tache");
            stream.writeTextElement("text",n.getTexte());
            if (n.getStatut()==attente) stream.writeTextElement("statut","Attente");
            else if (n.getStatut()==cours) stream.writeTextElement("statut","Cours");
            else stream.writeTextElement("statut","Terminee");
            stream.writeTextElement("deadline",n.getDeadline().QDate::toString(QString("dd/MM/yyyy")));
        }
        else if (typeid(*notes[i])==typeid(Fichier))
        {
            Fichier& n=dynamic_cast<Fichier&>(*notes[i]); stream.writeStartElement("fichier");
            stream.writeTextElement("description",n.getDescription());
            if (n.getType()==image) stream.writeTextElement("type","Image");
            else if (n.getType()==audio) stream.writeTextElement("type","Audio");
            else stream.writeTextElement("type","Video");
            stream.writeTextElement("filename",n.getFilename());
        }
            stream.writeTextElement("id",notes[i]->getId());
            stream.writeTextElement("title",notes[i]->getTitre());
            stream.writeTextElement("creation",notes[i]->getCreation().QDate::toString(QString("dd/MM/yyyy")));
            stream.writeTextElement("modification",notes[i]->getModification().QDate::toString(QString("dd/MM/yyyy")));
            if (notes[i]->IsLast()==true) stream.writeTextElement("last","oui");
            else stream.writeTextElement("last","non");
            if (notes[i]->getEtat()==active) stream.writeTextElement("etat","Active");
            else if (notes[i]->getEtat()==archivee) stream.writeTextElement("etat","Archivee");
            else stream.writeTextElement("etat","Corbeille");
            stream.writeTextElement("version",QString::number(notes[i]->getVersion()));
            stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeEndDocument();
    newfile.close();
}
