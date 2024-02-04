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

#ifndef _QCSTreeWidget22_H_
#define _QCSTreeWidget22_H_

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHash>

#include "ContinuousStructure.h"

#define PROPTYPE 0
#define PRIMTYPE 1

Q_DECLARE_METATYPE(CSProperties*);

class QCSTreeWidget2 : public QTreeWidget
{
	Q_OBJECT
public:
	QCSTreeWidget2(ContinuousStructure* CS, QWidget * parent = 0);
	virtual ~QCSTreeWidget2();
	
	CSProperties* GetCurrentProperty();
	CSPrimitives* GetCurrentPrimitive();
	
	void AddPrimItem(CSPrimitives* prim);
	void AddPropItem(CSProperties* prop);
	
	void DeletePrimItem(CSPrimitives* prim);
	void DeletePropItem(CSProperties* prop);
	
	void RefreshItem(int index);

	void UpdateTree();
	void ClearTree();

signals:
	void Edit();
	void Copy();
	void ShowHide();
	void Show(CSProperties* prop);
	void Hide(CSProperties* prop);

	void NewBox();
	void NewMultiBox();
	void NewSphere();
	void NewCylinder();	
	void NewUserDefined();

	void NewMaterial();
	void NewMetal();
	void NewExcitation();
	void NewChargeBox();
	void NewResBox();
	void NewDumpBox();

	void Delete();
	
public slots:
	void SwitchProperty(CSPrimitives* prim, CSProperties* newProp);
	
	void collapseAll();
	void expandAll();

protected slots:
	void Edit(QTreeWidgetItem * item, int column);
	void Clicked(QTreeWidgetItem * item, int column);
	
protected:
	void contextMenuEvent(QContextMenuEvent *event);

//	void BuildContextMenu();
	ContinuousStructure* clCS;
	
	QTreeWidgetItem* GetTreeItemByPrimID(int primID);
	int GetTreeItemIndexByPrimID(int primID);
	QVector<QTreeWidgetItem*> vPrimItems;
	
	
	enum QCSTreeWidget2ItemType {
		ROOT_NAME = 0,
		NAME,
		PRIM,
		PROP
	};

	class QCSTreeWidget2TreeNode
	{
	public:
		QCSTreeWidget2TreeNode()
			: item(nullptr), visible(true), prim(nullptr) {

		}
		QCSTreeWidget2TreeNode(QTreeWidgetItem* item)
			: item(item), visible(true), prim(nullptr) {
			
		}

		QTreeWidgetItem* item;
		bool visible;
		QHash<QString, QCSTreeWidget2TreeNode> children; // Inbetween nodes.
		CSPrimitives* prim; // Leaves.
	};
	
	QCSTreeWidget2TreeNode nameTreeRoot;
	QHash<QTreeWidgetItem*, QCSTreeWidget2TreeNode*> itemToNode;
	void expandNode(const QCSTreeWidget2TreeNode& node);
	void recursiveSetVisibility(QCSTreeWidget2TreeNode& node, bool visible);
	
};

#endif //_QCSTreeWidget22_H_
