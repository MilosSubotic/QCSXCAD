/*
*	Copyright (C) 2008,2009,2010 Thorsten Liebig (Thorsten.Liebig@gmx.de)
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU Lesser General Public License as published
*	by the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QMenu>
#include <QContextMenuEvent>

#include "QCSTreeWidget2.h"
#include "QCSXCAD_Global.h"

#include <QDebug>
#define TRACE() do{ qDebug() << __PRETTY_FUNCTION__; }while(0)
#define DEBUG(x) do{ qDebug() << #x << " = " << x; }while(0)

QCSTreeWidget2::QCSTreeWidget2(ContinuousStructure* CS, QWidget * parent) : QTreeWidget(parent)
{
	clCS=CS;
	setColumnCount(2);
	setColumnWidth(0,200);
//	setColumnWidth(1,16);
	setHeaderLabels(QStringList(tr("Name / Prop / Prims"))<<tr("Vis"));
//	setItemHidden(headerItem(),true);
//	qTree->setDragEnabled(true);
//	qTree->setAcceptDrops(true);
//	qTree->setDropIndicatorShown(true);
	QObject::connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(Edit(QTreeWidgetItem*,int)));
	QObject::connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(Clicked(QTreeWidgetItem*,int)));
}

QCSTreeWidget2::~QCSTreeWidget2()
{
}

CSProperties* QCSTreeWidget2::GetCurrentProperty()
{
TRACE();
	QTreeWidgetItem* curr=currentItem();
	if (curr==NULL) return NULL;
	if (curr->type()==PRIMTYPE) curr=curr->parent();
	if (curr==NULL) return NULL;
	return clCS->GetProperty(indexOfTopLevelItem(curr));
}

CSPrimitives* QCSTreeWidget2::GetCurrentPrimitive()
{
TRACE();
	QTreeWidgetItem* curr=currentItem();
	if (curr==NULL) return NULL;
	if (curr->type()!=PRIMTYPE) return NULL;
	return clCS->GetPrimitiveByID(curr->data(0,1).toInt());
}

void QCSTreeWidget2::AddPrimItem(CSPrimitives* prim)
{
	if (prim==NULL)
		return;
#if 0
	int propID=clCS->GetIndex(prim->GetProperty());
	if (propID<0)
		return;
	QTreeWidgetItem* parent = topLevelItem(propID);
	if (parent==NULL)
		return;
	QString str = QString(prim->GetTypeName().c_str());

	str+=QString(" - ID: %1").arg(prim->GetID());
	QTreeWidgetItem* newPrimItem = new QTreeWidgetItem(parent,QStringList(str),1);
	newPrimItem->setData(0,1,QVariant(prim->GetID()));
	vPrimItems.push_back(newPrimItem);
#else
	// Recusive add to primitve tree
	QString full_name=QString::fromUtf8(prim->GetName().c_str());
	QStringList nameList=full_name.split('/');

	int depth=0;
	QCSTreeWidget2TreeNode* node=&nameTreeRoot;
	for (const auto& name : nameList) {
		if (!node->children.contains(name))
		{
			QTreeWidgetItem* newItem;
			if (depth==0)
			{
				newItem=new QTreeWidgetItem(this,QStringList(name),ROOT_NAME);
			}
			else
			{
				newItem=new QTreeWidgetItem(node->item,QStringList(name),NAME);
			}
			node->children.insert(name,QCSTreeWidget2TreeNode(newItem));
			itemToNode[newItem]=&node->children[name];
			
			newItem->setFont(0,QFont("Arial",10));
			if (node->children[name].visible) newItem->setIcon(1,QIcon(":/images/bulb.png"));
			else newItem->setIcon(1,QIcon(":/images/bulb_off.png"));
		}
		node = &node->children[name];
		
		depth++;
	}
	
	QTreeWidgetItem* item=node->item;
	
	if (item==NULL)
		return;
	
	
	
	node->prim=prim;
	
	QString str=QString(prim->GetTypeName().c_str());
	str+=QString(" - ID: %1").arg(prim->GetID());
	QTreeWidgetItem* newPrimItem=new QTreeWidgetItem(item,QStringList(str),PRIM);
	newPrimItem->setData(0,1,QVariant(prim->GetID()));
	vPrimItems.push_back(newPrimItem);
	
	CSProperties* prop=prim->GetProperty();
	if (prop==NULL) return;
	str=QString(prop->GetTypeXMLString().c_str())+"::";
	str+=QString::fromUtf8(prop->GetName().c_str());
	QTreeWidgetItem* newPropItem=new QTreeWidgetItem(item,QStringList(str),PROP);
	
	//TODO Go up and do hide if prop is not visible?
#endif
}

void QCSTreeWidget2::AddPropItem(CSProperties* prop)
{
#if 0
	QString str;
	if (prop==NULL) return;
	str=QString(prop->GetTypeXMLString().c_str())+"::";
	str+=QString::fromUtf8(prop->GetName().c_str());
		
	QTreeWidgetItem* newItem = new QTreeWidgetItem(this,QStringList(str),0);
	newItem->setFont(0,QFont("Arial",10));
	if (prop->GetVisibility()) newItem->setIcon(1,QIcon(":/images/bulb.png"));
	else newItem->setIcon(1,QIcon(":/images/bulb_off.png"));
#elif 0
	if (prop==NULL) return;
	DEBUG(QString::fromUtf8(prop->GetName().c_str()));
	QString propType=QString(prop->GetTypeXMLString().c_str());
	QString name=QString::fromUtf8(prop->GetName().c_str());
	QStringList nameList=name.split('/');

	int depth = 0;
	QCSTreeWidget2TreeNode* node = &nameTreeRoot;
	for (const auto& name : nameList) {
		DEBUG(name);
		if (!node->children.contains(name))
		{
			DEBUG("!contains");
			QTreeWidgetItem* item = 0;
			if (depth == 0)
			{
				DEBUG(0);
				item = new QTreeWidgetItem(this, QStringList(name), PROPTYPE);
				DEBUG(item);
			}
			else
			{
				DEBUG(1);
				DEBUG(node->item);
				item = new QTreeWidgetItem(node->item, QStringList(name), PRIMTYPE);
				DEBUG(item);
			}
			//item->setFont(0,QFont("Arial",10));
			node->children.insert(name, QCSTreeWidget2TreeNode(item));
		}
		node = &node->children[name];
		
		depth++;
	}
#else
	// When primitive is added, than property is added.
#endif
}
	

QTreeWidgetItem* QCSTreeWidget2::GetTreeItemByPrimID(int primID)
{
TRACE();
	for (int n=0;n<vPrimItems.size();++n)
		if (vPrimItems.at(n)->data(0,1).toInt()==primID)
			return vPrimItems.at(n);
	return NULL;
}

int QCSTreeWidget2::GetTreeItemIndexByPrimID(int primID)
{
TRACE();
	for (int n=0;n<vPrimItems.size();++n)
		if (vPrimItems.at(n)->data(0,1).toInt()==primID)
			return n;
	return -1;
}

void QCSTreeWidget2::DeletePrimItem(CSPrimitives* prim)
{
TRACE();
	int index=GetTreeItemIndexByPrimID(prim->GetID());
	QTreeWidgetItem* item;
	if ((index>=0) && (index<vPrimItems.size()))
		item=vPrimItems.at(index);
	else return;
	vPrimItems.remove(index);
	delete item;
}

void QCSTreeWidget2::DeletePropItem(CSProperties* prop)
{
TRACE();
	int index=clCS->GetIndex(prop);
	
	QTreeWidgetItem* parent = topLevelItem(index);
	if (parent==NULL) return;
	
	delete parent;
}

void QCSTreeWidget2::RefreshItem(int index)
{
TRACE();
	CSProperties* prop=clCS->GetProperty(index);
	if (prop==NULL) return;
	//TODO This need to be changed, but later, not called for now.
	QTreeWidgetItem* item = topLevelItem(index);
	if (item==NULL) return;
	QString str=QString(prop->GetTypeXMLString().c_str())+"::";
	str+=QString::fromUtf8(prop->GetName().c_str());
	item->setText(0,str);	
	if (prop->GetVisibility()) item->setIcon(1,QIcon(":/images/bulb.png"));
	else item->setIcon(1,QIcon(":/images/bulb_off.png"));
}


void QCSTreeWidget2::contextMenuEvent(QContextMenuEvent *event)
{
TRACE();
	QMenu menu(this);
	if (QCSX_Settings.GetEdit())
	{
		QMenu* primM = menu.addMenu(QIcon(":/images/edit_add.png"),tr("New Primitive"));
		primM->addAction(tr("Box"),this,SIGNAL(NewBox()));
		primM->addAction(tr("Multi-Box"),this,SIGNAL(NewMultiBox()));
		primM->addAction(tr("Sphere"),this,SIGNAL(NewSphere()));
		primM->addAction(tr("Cylinder"),this,SIGNAL(NewCylinder()));
		primM->addAction(tr("User Defined"),this,SIGNAL(NewUserDefined()));

		QMenu* propM = menu.addMenu(QIcon(":/images/edit_add.png"),tr("New Property"));
		propM->addAction(tr("Material"),this,SIGNAL(NewMaterial()));
		propM->addAction(tr("Metal"),this,SIGNAL(NewMetal()));
		propM->addAction(tr("Excitation"),this,SIGNAL(NewExcitation()));
		propM->addAction(tr("Probe-Box"),this,SIGNAL(NewChargeBox()));
		propM->addAction(tr("Res-Box"),this,SIGNAL(NewResBox()));
		propM->addAction(tr("Dump-Box"),this,SIGNAL(NewDumpBox()));

		menu.addSeparator();

		menu.addAction(QIcon(":/images/edit.png"),tr("Edit"),this,SIGNAL(Edit()));
		menu.addAction(QIcon(":/images/editcopy.png"),tr("Copy"),this,SIGNAL(Copy()));
		menu.addAction(QIcon(":/images/edit_remove.png"),tr("Delete"),this,SIGNAL(Delete()));
	}
	else
	{
		menu.addAction(QIcon(":/images/edit.png"),tr("View"),this,SIGNAL(Edit()));
	}

	menu.exec(event->globalPos());
}

void QCSTreeWidget2::UpdateTree()
{
	ClearTree();
	for (size_t i=0;i<clCS->GetQtyProperties();++i)
	{
		QString str;
		CSProperties* prop=clCS->GetProperty(i);
		if (prop==NULL) break;
		AddPropItem(prop);
	}

	std::vector<CSPrimitives*> vPrims = clCS->GetAllPrimitives();
	for (size_t i=0;i<vPrims.size();++i)
		AddPrimItem(vPrims.at(i));
}

void QCSTreeWidget2::ClearTree()
{
	this->clear();
	vPrimItems.clear();
	nameTreeRoot=QCSTreeWidget2TreeNode();
	itemToNode.clear();
}

void QCSTreeWidget2::SwitchProperty(CSPrimitives* prim, CSProperties* newProp)
{
TRACE();
	int index=GetTreeItemIndexByPrimID(prim->GetID());
	QTreeWidgetItem *item=NULL;
	if ((index>=0) && (index<vPrimItems.size()))
		item=vPrimItems.at(index);
	else
		return;

	QTreeWidgetItem *parent=item->parent();
	if (parent==NULL)
		return;
	QTreeWidgetItem *newParent = topLevelItem(clCS->GetIndex(newProp));
	if (newParent==NULL)
		return;

	parent->takeChild(parent->indexOfChild(item));
	newParent->addChild(item);
}

void QCSTreeWidget2::collapseAll()
{
TRACE();
	for (int i=0;i<topLevelItemCount();++i) collapseItem(topLevelItem(i));
}

void QCSTreeWidget2::expandAll()
{
#if 0
	for (int i=0;i<topLevelItemCount();++i) expandItem(topLevelItem(i));
#else
	expandNode(nameTreeRoot);
#endif
}

void QCSTreeWidget2::Edit(QTreeWidgetItem * item, int column)
{
TRACE();
	UNUSED(column);
	if (item==NULL) return;
	if (item->type()!=PRIMTYPE) return;
	emit Edit();
}

void QCSTreeWidget2::Clicked(QTreeWidgetItem * item, int column)
{
#if 0
	if (item==NULL) return;
	if (item->type()!=PROPTYPE) return;
	if (column==1) {emit ShowHide();}
#else
	if (item==NULL) return;
	if (item->type()==ROOT_NAME || item->type()==NAME)
		if (column==1)
		{
			auto node=itemToNode[item];
			recursiveSetVisibility(*node, !node->visible);
		}
#endif
}

void QCSTreeWidget2::expandNode(const QCSTreeWidget2TreeNode& node) {
	expandItem(node.item);
	
	for (auto it=node.children.cbegin(), end=node.children.cend(); it!=end; it++)
	{
		expandNode(it.value());
	}
}

void QCSTreeWidget2::recursiveSetVisibility(QCSTreeWidget2TreeNode& node, bool visible) {
	node.visible=visible;
	if (node.item)
	{
		if (visible) node.item->setIcon(1,QIcon(":/images/bulb.png"));
		else node.item->setIcon(1,QIcon(":/images/bulb_off.png"));
	}
	
	if (node.prim)
	{
		CSProperties* prop=node.prim->GetProperty();
		if (visible) emit Show(prop);
		else emit Hide(prop);
	}
	
	for (auto it=node.children.begin(), end=node.children.end(); it!=end; it++)
	{
		recursiveSetVisibility(it.value(), visible);
	}
}
