#include<interface.h>
#include<typeinfo>
#include<QGroupBox>
#include<QFormLayout>

/**
 * \file      interface.cpp
 * \brief     Définit les méthodes relatives à l'interface de l'application.
 *
 * \details    Définition des constructeurs des fenêtres principale & secondaire ainsi que toutes les méthodes qui y sont
 * nécessaires, et l'ensemble des slots nécessaires au fonctionnement de l'application.
 */

//=============================FENÊTRE PRINCIPALE======================================



/*VuePrincipale& VuePrincipale::getInstance(Note* n){
    static VuePrincipale instance(n);
    return instance;
}*/

VuePrincipale::VuePrincipale() : note(nullptr),marqueur(0){
    zoneCentrale=new QWidget;
    setCentralWidget(zoneCentrale);
    affichage_gauche();
    affichage_central();
    affichage_droit();
    creerMenu();
    //Définition du layout principal
        layoutPrincipal = new QHBoxLayout;
        layoutPrincipal->addWidget(gauche);
        layoutPrincipal->addWidget(centre);
        layoutPrincipal->addWidget(droite);
        zoneCentrale->setLayout(layoutPrincipal);
        setWindowTitle("Pluri'notes");
}

void VuePrincipale::creerMenu(){
    QMenu* menuFichier = menuBar()->addMenu("&Fichier");
       QMenu *nouveau = menuFichier->addMenu("&Nouveau");
           QAction *actionArt=new QAction("Nouvel &article", this);
           nouveau->addAction(actionArt);
           connect(actionArt, SIGNAL(triggered()), this, SLOT(creerArticle()));
           QMenu* actionTache=nouveau->addMenu("Nouvelle &tâche");
                QAction* actionTacheSimple=new QAction("Tâche simple", this);
                actionTache->addAction(actionTacheSimple);
                connect(actionTacheSimple, SIGNAL(triggered()), this, SLOT(creerTache()));
                QAction* actionDeadline=new QAction("Tâche avec deadline", this);
                actionTache->addAction(actionDeadline);
                connect(actionDeadline, SIGNAL(triggered()), this, SLOT(creerTacheDeadline()));
                QAction* actionPrio=new QAction("Tâche avec priorité", this);
                actionTache->addAction(actionPrio);
                connect(actionPrio, SIGNAL(triggered()), this, SLOT(creerTachePriorite()));
           QAction* actionFichier=new QAction("Nouveau &fichier", this);
           nouveau->addAction(actionFichier);
           connect(actionFichier, SIGNAL(triggered()), this, SLOT(creerFichier()));
        /*QAction* actionSave=new QAction("&Enregistrer", this);
        menuFichier->addAction(actionSave);
        actionSave->setShortcut(QKeySequence("Ctrl+S"));
        actionSave->setDisabled(true);
        connect(actionSave, SIGNAL(triggered()), this, SLOT(interfaceSave()));
        QObject::connect(noteEdit->getTitle(), SIGNAL(textChanged(QString)), this, SLOT(activerMenuSave(QString)));*/
        QAction *actionQuitter = new QAction("&Quitter", this);
        actionQuitter->setShortcut(QKeySequence("Ctrl+Q"));
        connect(actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));
        menuFichier->addAction(actionQuitter);


    QMenu *menuEdition = menuBar()->addMenu("&Edition");
       QAction* actionAnnuler=new QAction("Annuler", this);
       menuEdition->addAction(actionAnnuler);
       actionAnnuler->setShortcut(QKeySequence("Ctrl+Z"));
       QAction* actionRetablir=new QAction("Rétablir", this);
       actionRetablir->setShortcut(QKeySequence("Ctrl+Y"));
       menuEdition->addAction(actionRetablir);

    QMenu *menuCorbeille = menuBar()->addMenu("&Corbeille");
       actionSupp=new QAction("Supprimer automatiquement les notes archivées à la fermeture de l'application", this);
       menuCorbeille->addAction(actionSupp);
       actionSupp->setCheckable(true);
}


void VuePrincipale::closeEvent(QCloseEvent* event){
    NotesManager& manager=NotesManager::getInstance();
    if (actionSupp->isChecked())
    {
        manager.setVidage(true);
        event->accept();
    }
    else
    {
        int ret = QMessageBox::question(this,"Vider la corbeille","Voulez-vous vider la corbeille ?",QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) manager.setVidage(true);
                else manager.setVidage(false);
        event->accept();
    }
}



void VuePrincipale::afficageArbo(){
    if(arboVisible){
        arborescence->setText("Voir l'arborescence");
        droite->hide();
        arboVisible=false;
    }
    else{
        arborescence->setText("Masquer l'arborescence");
        arboVisible=true;
        droite->show();
    }
}

void VuePrincipale::showRelations(){
    VueSecondaire* fenetreRelations= new VueSecondaire;
    fenetreRelations->show();
}

void VuePrincipale::afficher_note(QTreeWidgetItem *item){
    NotesManager& manager=NotesManager::getInstance();
    note = &manager.getNote(item->text(0));
    delete centre;
    delete droite;
    affichage_central();
    affichage_droit();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);
}


void VuePrincipale::creerArticle(){
    marqueur=1;
    note=nullptr;
    delete centre;
    delete droite;
    affichage_droit();
    affichage_central();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);
}
void VuePrincipale::creerTache(){
    marqueur=2; note=nullptr;
    delete centre;
    delete droite;
    affichage_droit();
    affichage_central();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);
}
void VuePrincipale::creerTacheDeadline(){
    marqueur=4; note=nullptr;
    delete centre;
    delete droite;
    affichage_droit();
    affichage_central();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);
}
void VuePrincipale::creerTachePriorite(){
    marqueur=3; note=nullptr;
    delete centre;
    delete droite;
    affichage_droit();
    affichage_central();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);
}
void VuePrincipale::creerFichier(){
    marqueur=5; note=nullptr;
    delete centre;
    delete droite;
    affichage_droit();
    affichage_central();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);
}


void VuePrincipale::afficher_version(QTreeWidgetItem *item){
    NotesManager& manager=NotesManager::getInstance();
    bool conversion;
    int version=item->text(0).toUInt(&conversion, 10);
    QString id=note->getId();
    note = &manager.getVersionNote(id,version);
    delete centre;
    delete droite;
    affichage_central();
    affichage_droit();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);
}

void VuePrincipale::affichage_central()
{
    if(note==nullptr && marqueur==0)
    {
        centre=new QGroupBox("Nouvelle note", zoneCentrale);
        QVBoxLayout* button_layout=new QVBoxLayout;
        QSignalMapper* mapper_button = new QSignalMapper(this);
        connect(mapper_button, SIGNAL(mapped(int)), this, SLOT(new_note(int)));

        QPushButton* article=new QPushButton("Nouvel Article", this);
        connect(article, SIGNAL(clicked()), mapper_button, SLOT(map())); mapper_button->setMapping(article,1);
        QPushButton* tache=new QPushButton("Nouvelle Tache", this);
        connect(tache, SIGNAL(clicked()), mapper_button, SLOT(map())); mapper_button->setMapping(tache,2);
        QPushButton* tache_p=new QPushButton("Nouvelle Tache avec priorite", this);
        connect(tache_p, SIGNAL(clicked()), mapper_button, SLOT(map())); mapper_button->setMapping(tache_p,3);
        QPushButton* tache_d=new QPushButton("Nouvelle Tache avec deadline", this);
        connect(tache_d, SIGNAL(clicked()), mapper_button, SLOT(map())); mapper_button->setMapping(tache_d,4);
        QPushButton* fichier=new QPushButton("Nouveau Fichier", this);
        connect(fichier, SIGNAL(clicked()), mapper_button, SLOT(map()));mapper_button->setMapping(fichier,5);
        button_layout->addWidget(article); button_layout->addWidget(tache); button_layout->addWidget(tache_p); button_layout->addWidget(tache_d); button_layout->addWidget(fichier);
        centre->setLayout(button_layout);
    }
    else {
        if (marqueur==0)
        {
        if(typeid(*note)==typeid(Article))
            {
            Article& a=dynamic_cast<Article&>(*note);
            noteEdit=new ArticleEditeur(a);
            }
            else if(typeid(*note)==typeid(TacheAvecPriorite))
                {
                TacheAvecPriorite& t=dynamic_cast<TacheAvecPriorite&>(*note);
                noteEdit=new TacheAvecPrioriteEditeur(t);
                }
                else if (typeid(*note)==typeid(TacheAvecDeadline))
                    {
                    TacheAvecDeadline& t=dynamic_cast<TacheAvecDeadline&>(*note);
                    noteEdit=new TacheAvecDeadlineEditeur(t);
                    }
                    else if(typeid(*note)==typeid(Tache))
                        {
                            Tache& t=dynamic_cast<Tache&>(*note);
                            noteEdit=new TacheEditeur(t);
                        }
                        else if(typeid(*note)==typeid(Fichier))
                            {
                            Fichier& f=dynamic_cast<Fichier&>(*note);
                            noteEdit=new FichierEditeur(f);
                            }
                            else throw NotesException("Ce type de note n'existe pas.");
        }
        else
        {
            switch (marqueur)
        {
        case 1 : noteEdit=new ArticleEditeur; break;
        case 2 : noteEdit=new TacheEditeur; break;
        case 3 : noteEdit=new TacheAvecPrioriteEditeur; break;
        case 4 : noteEdit=new TacheAvecDeadlineEditeur; break;
        case 5 : noteEdit=new FichierEditeur; break;
        default : throw NotesException("Valeur de marqueur invalide");
        }
        marqueur=0;
        }
        QFormLayout* editeur=new QFormLayout;
        editeur->addRow("", noteEdit);
        centre=new QGroupBox("Visualisation de la note", zoneCentrale);
        centre->setLayout(editeur);
    }
}

void VuePrincipale::affichage_droit()
{
          droite=new QGroupBox("Arborescence", zoneCentrale);
          QVBoxLayout* rightLayout=new QVBoxLayout;
          if (note==nullptr) {
              QLabel* label=new QLabel(this); label->setText("Pas d'arborescence disponible.");
              rightLayout->addWidget(label);
          }
          else {
          if (note->IsLast())
          {
              QTreeWidget* liste_relations=new QTreeWidget(this);
              QStringList header;
              header<<"Relations"<<"";
              liste_relations->setHeaderLabels(header);
              liste_relations->setColumnCount(2);
              QObject::connect(liste_relations, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(afficher_note(QTreeWidgetItem*)));

              QTreeWidgetItem* arbreAscendants = new QTreeWidgetItem(liste_relations);
              arbreAscendants->setText(0,"Ascendants");
              arbreAscendants->setExpanded(true);
              liste_relations->addTopLevelItem(arbreAscendants);

               QTreeWidgetItem* arbreDescendants = new QTreeWidgetItem(liste_relations);
               arbreDescendants->setText(0,"Descendants");
               arbreDescendants->setExpanded(true);
               liste_relations->addTopLevelItem(arbreDescendants);

               unsigned int nb_items_relations=2;
               QTreeWidgetItem** item = new QTreeWidgetItem*[nb_items_relations];
               unsigned int i=0;
               RelationsManager::Iterator iterator_manager=RelationsManager::getInstance().getIterator();
               iterator_manager.debut();
               while(!iterator_manager.isDone()){ //ici on examine chaque relation
                  Relation::Iterator iterator_relation= iterator_manager.current().getIterator();
                  iterator_relation.debut();
                  while(!iterator_relation.isDone()) //ici on examine chaque couple de la relation
                  {
                      if(i>nb_items_relations-2) //il reste moins de 2 cases dans le tableau d'items, il faut l'agrandir
                      {
                          QTreeWidgetItem** new_item = new QTreeWidgetItem*[nb_items_relations+10];
                          for (unsigned int k=0; k<nb_items_relations; k++)
                              new_item[k] = item[k];
                          QTreeWidgetItem** old_item = item;
                          item=new_item;
                          delete [] old_item;
                          nb_items_relations+=10;
                      }
                      if(&(iterator_relation.current_noteX())==note){
                          item[i] = new QTreeWidgetItem();
                          item[i]->setText(0, iterator_relation.current_noteY().getId());
                          item[i]->setText(1,"("+iterator_manager.current().getTitre()+")");
                          arbreDescendants->addChild(item[i]);
                          i++;
                      }
                      if(&(iterator_relation.current_noteY())==note){
                           item[i] = new QTreeWidgetItem();
                           item[i]->setText(0, iterator_relation.current_noteX().getId());
                           item[i]->setText(1,"("+iterator_manager.current().getTitre()+")");
                           arbreAscendants->addChild(item[i]);
                           i++;
                      }
                      iterator_relation.next();
                  }
                  iterator_manager.next();
                }
               rightLayout->addWidget(liste_relations);
          }
          NotesManager& manager=NotesManager::getInstance();
          unsigned int nb_versions=manager.getNote(note->getId()).getVersion();
          if(nb_versions>1)
          {
              QTreeWidget* liste_versions=new QTreeWidget(this);
              liste_versions->setHeaderLabels(QStringList("Versions"));
              QObject::connect(liste_versions, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(afficher_version(QTreeWidgetItem*)));
              QTreeWidgetItem** item_version = new QTreeWidgetItem*[nb_versions];
              for (unsigned int i=0; i<=nb_versions-1; i++)
              {
                  item_version[i] = new QTreeWidgetItem();
                  item_version[i]->setText(0, QString::number(i+1));
                  liste_versions->addTopLevelItem(item_version[i]);
              }
              rightLayout->addWidget(liste_versions);

          }
}
          arboVisible=true;
          relation_details=new QPushButton("Gerer les relations", this);
          rightLayout-> addWidget(relation_details);
          droite->setLayout(rightLayout);
          connect(relation_details, SIGNAL(clicked()), this, SLOT(showRelations()));
}

void VuePrincipale::affichage_gauche(){
    NotesManager& manager=NotesManager::getInstance();
    NotesManager::Iterator iterator=manager.getIterator();
    QVBoxLayout* leftLayout=new QVBoxLayout;
    QTreeWidget* notesActives=new QTreeWidget(this);
    notesActives->setHeaderLabels(QStringList("Actives"));
    QObject::connect(notesActives, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(afficher_note(QTreeWidgetItem*)));
    QTreeWidget* taches=new QTreeWidget(this);
    QStringList header;
    header<<"Taches"<<"";
    taches->setHeaderLabels(header);
    taches->setColumnCount(2);
    QObject::connect(taches, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(afficher_note(QTreeWidgetItem*)));
    QTreeWidget* archives=new QTreeWidget(this);
    archives->setHeaderLabels(QStringList("Archives"));
    QObject::connect(archives, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(afficher_note(QTreeWidgetItem*)));
    unsigned int nb_items_relations=2;
    QTreeWidgetItem** item = new QTreeWidgetItem*[nb_items_relations];
    unsigned int i=0;
    for (iterator.debut(); !iterator.isDone(); iterator.next())
    {
        if(i==nb_items_relations)
        {
            QTreeWidgetItem** new_item = new QTreeWidgetItem*[nb_items_relations+10];
            for (unsigned int k=0; k<nb_items_relations; k++)
                new_item[k] = item[k];
            QTreeWidgetItem** old_item = item;
            item=new_item;
            delete [] old_item;
            nb_items_relations+=10;
        }

        if (iterator.current().IsLast())
        {
            item[i]=new QTreeWidgetItem();
            item[i]->setText(0, iterator.current().getId());
            if (iterator.current().getEtat()==active)
            {
                if (typeid(iterator.current())==typeid(Tache) || typeid(iterator.current())==typeid(TacheAvecPriorite) || typeid(iterator.current())==typeid(TacheAvecDeadline))
                {
                    if (typeid(iterator.current())==typeid(TacheAvecPriorite))
                        {TacheAvecPriorite& t=dynamic_cast<TacheAvecPriorite&>(iterator.current()); item[i]->setText(1, "(priorite : "+QString::number(t.getPriorite())+")");}
                    if (typeid(iterator.current())==typeid(TacheAvecDeadline))
                        {TacheAvecDeadline& t=dynamic_cast<TacheAvecDeadline&>(iterator.current()); item[i]->setText(1, "(deadline : "+t.getDeadline().QDate::toString(QString("dd/MM/yyyy"))+")");}
                    taches->addTopLevelItem(item[i]);
                }
                else notesActives->addTopLevelItem(item[i]);
            }
            else if (iterator.current().getEtat()==archivee)
                archives->addTopLevelItem(item[i]);
        }
    }

    arborescence=new QPushButton("Masquer l'arborescence", this);
    actualiser=new QPushButton("Actualiser", this);
    leftLayout->addWidget(notesActives);
    leftLayout->addWidget(taches);
    leftLayout->addWidget(archives);
    leftLayout->addWidget(arborescence);
    leftLayout->addWidget(actualiser);
    gauche=new QGroupBox("Notes", zoneCentrale);
    gauche->setLayout(leftLayout);

    connect(arborescence, SIGNAL(clicked()), this, SLOT(afficageArbo()));
    connect(actualiser, SIGNAL(clicked()), this, SLOT(actualiser_fenetre()));
}

void VuePrincipale::new_note(int i)
{
    marqueur=i;
    delete centre;
    delete droite;
    affichage_central();
    affichage_droit();
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);

}

void VuePrincipale::actualiser_fenetre()
{
    note=nullptr;
    delete gauche;
    delete centre;
    delete droite;
    affichage_gauche();
    affichage_central();
    affichage_droit();
    layoutPrincipal->addWidget(gauche);
    layoutPrincipal->addWidget(centre);
    layoutPrincipal->addWidget(droite);
    zoneCentrale->setLayout(layoutPrincipal);

}


//==========================================FENÊTRE SECONDAIRE (RELATIONS)========================================

/*VueSecondaire& VueSecondaire::getInstance(){
    static VueSecondaire instance;
    return instance;
}*/

VueSecondaire::VueSecondaire():relation(nullptr){
    layoutPrincipal=new QHBoxLayout;
    affichage_gauche();
    affichage_central();
    layoutPrincipal->addWidget(gauche);
    layoutPrincipal->addWidget(centre);
    setLayout(layoutPrincipal);

    setWindowTitle("Gestion des relations");
    resize(350, 450);

}

void VueSecondaire::affichage_gauche(){
    RelationsManager& manager=RelationsManager::getInstance();
    RelationsManager::Iterator iterator=manager.getIterator();
    QVBoxLayout* leftLayout=new QVBoxLayout;
    QTreeWidget* relations=new QTreeWidget(this);
    relations->setHeaderLabels(QStringList("Relations"));
    QObject::connect(relations, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(openRelation(QTreeWidgetItem*)));
    unsigned int nb_items_relations=0;
    QTreeWidgetItem** item = new QTreeWidgetItem*[nb_items_relations];
    unsigned int i=0;
    for (iterator.debut(); !iterator.isDone(); iterator.next())
    {
        if(i==nb_items_relations)
        {
            QTreeWidgetItem** new_item = new QTreeWidgetItem*[nb_items_relations+10];
            for (unsigned int k=0; k<nb_items_relations; k++)
                new_item[k] = item[k];
            QTreeWidgetItem** old_item = item;
            item=new_item;
            delete [] old_item;
            nb_items_relations+=10;
        }
        item[i]=new QTreeWidgetItem();
        item[i]->setText(0, iterator.current().getTitre());
        relations->addTopLevelItem(item[i]);
    }

    quitter=new QPushButton("Quitter", this);
    connect(quitter, SIGNAL(clicked()), this, SLOT(close()));
    actualiser=new QPushButton("Actualiser / Nouvelle relation", this);
    connect(actualiser, SIGNAL(clicked()), this, SLOT(actualiser_fenetre()));
    leftLayout->addWidget(quitter);
    leftLayout->addWidget(relations);
    leftLayout->addWidget(actualiser);
    gauche=new QGroupBox;
    gauche->setLayout(leftLayout);

}

void VueSecondaire::affichage_central()
{
    if(relation==nullptr)
    {
        QFormLayout* editeur=new QFormLayout;
        relationEdit=new RelationEditeur;
        editeur->addRow("", relationEdit);
        centre=new QGroupBox("Editeur d'une nouvelle relation");
        centre->setLayout(editeur);
    }
    else {
        QFormLayout* editeur=new QFormLayout;
        relationEdit=new RelationEditeur(*relation);
        editeur->addRow("", relationEdit);
        centre=new QGroupBox("Editeur de la relation");
        centre->setLayout(editeur);
    }
}

void VueSecondaire::openRelation(QTreeWidgetItem* item){
    RelationsManager& manager=RelationsManager::getInstance();
    RelationsManager::Iterator it=manager.getIterator();
    while(!it.isDone() && it.current().getTitre()!=item->text(0)) it.next();
    if (it.isDone() && it.current().getTitre()!=item->text(0)) throw InterfaceException("Erreur, cette relation n'existe pas");
    relation = &it.current();
    delete centre;
    affichage_central();
    layoutPrincipal->addWidget(centre);
    setLayout(layoutPrincipal);
}

void VueSecondaire::actualiser_fenetre()
{
    relation=nullptr;
    delete gauche;
    delete centre;
    affichage_gauche();
    affichage_central();
    layoutPrincipal->addWidget(gauche);
    layoutPrincipal->addWidget(centre);
    setLayout(layoutPrincipal);

}

/*QFormLayout* editeur=new QFormLayout;
editeur->addRow("", noteEdit);
centre=new QGroupBox("Visualisation de la note", zoneCentrale);
centre->setLayout(editeur);*/

/*void VuePrincipale::activerMenuSave(QString str){
    actionSave->setDisabled(false);
}*/
