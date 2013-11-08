/*
	*** Variable Node
	*** src/parser/variablenode.cpp
	Copyright T. Youngs 2010-2013

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "parser/variablenode.h"
#include "parser/returnvalue.h"
#include "parser/variable.h"
#include "parser/stepnode.h"
#include <string.h>

// Constructor
VariableNode::VariableNode(Variable *var) : variable_(var)
{
	// Private variables
	readOnly_ = false;
	nodeType_ = TreeNode::VarWrapperNode;
	if (variable_ != NULL) returnType_ = var->returnType();
}

// Destructor
VariableNode::~VariableNode()
{
}

// Set function
void VariableNode::setVariable(Variable *variable)
{
	variable_ = variable;
}

// Get function
Variable *VariableNode::variable()
{
	return variable_;
}

// Return name of variable target
const char *VariableNode::name()
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer.\n");
		return "NULL";
	}
	return variable_->name();
}

// Finalise path, setting return value and readOnly property from last step node
void VariableNode::finalisePath()
{
	// Return type of last argument is return type of PathNode
	if (args_.last() == NULL) returnType_ = VTypes::NoData;
	else
	{
		StepNode *step = (StepNode*) args_.last()->item;
		returnType_ = step->returnType();
		readOnly_ = step->readOnly();
	}
}

/*
// Set / execute
*/

// Execute command
bool VariableNode::execute(ReturnValue &rv)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be executed.\n");
		return false;
	}

	// Call the local variable's execute() function to get the base value
	bool result;
	result = variable_->execute(rv);

	// If a path is present (i.e. there are arguments to the VariableNode, then execute it. Otherwise, just return the variable contents
	// Next, step through accessnodes, passing the returnvalue to each in turn
	if (result) for (RefListItem<TreeNode,int> *ri = args_.first(); ri != NULL; ri = ri->next)
	{
		result = ri->item->execute(rv);
		if (!result) break;
	}
	if (result)
	{
// 		printf("Final result of path walk / variable retrieval is:\n");
// 		rv.info();
	}
	else printf("Variable retrieval failed.\n");
	return result;
}

// Print node contents
void VariableNode::nodePrint(int offset, const char *prefix)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be printed.\n");
		return;
	}
	// Call the local variables nodePrint() function
	variable_->nodePrint(offset, prefix);
	// If there is a path, print that as well
	if (args_.nItems() != 0)
	{
		// Construct tabbed offset
		offset++;

		Dnchar tab(offset+32);
		for (int n=0; n<offset-1; n++) tab += '\t';
		if (offset > 1) tab.strcat("   |--> ");
		tab.strcat(prefix);

		printf("[PATH]%s (basevar).", tab.get());
		for (RefListItem<TreeNode,int> *ri = args_.first(); ri != NULL; ri = ri->next)
		{
			ri->item->nodePrint(offset);
			if (ri->next != NULL) printf(".");
			else printf(" [%s]\n", VTypes::dataType(returnType_));
		}
	}
}

// Set from returnvalue node
bool VariableNode::set(ReturnValue setrv)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be set.\n");
		return false;
	}
	bool result = true;
	ReturnValue executerv, lastresult;
	// If there are no path nodes then just set the local variable
	if (args_.nItems() == 0)
	{
		// Call the local variable's set() function
		result = variable_->set(setrv);
	}
	else
	{
		// Call the local variable's execute() function to get the base value
		bool result2;
		result2 = variable_->execute(executerv);

		// Next, step through accessnodes up until the last one, passing the returnvalue to each in turn.
		if (result2)
		{
			lastresult = executerv;
			for (RefListItem<TreeNode,int> *ri = args_.first(); ri != args_.last(); ri = ri->next)
			{
				result = ri->item->execute(executerv);
				if (!result) break;
			}
			// For the last accessnode in the list, cast into a StepNode and use the set function
// 			printf("Node type of args_>last() is %i\n", args_.last()->item->nodeType());
// 			printf("Penultimate result is %s\n", lastresult.info());
			if (!((StepNode*) args_.last()->item)->set(executerv, setrv)) result = false;
		}
		else result = false;
	}
	if (!result) printf("Variable set failed.\n");
	return result;
}

// Initialise node
bool VariableNode::initialise()
{
	if (variable_ == NULL)
	{
		printf("Internal Error: VariableNode contains a NULL Variable pointer and can't be initialised.\n");
		return false;
	}
	return variable_->initialise();
}

// Search accessors (if any) available for linked variable
StepNode *VariableNode::findAccessor(const char* s, TreeNode* arglist)
{
	if (variable_ == NULL)
	{
		printf("Internal Error: No variable stored in VariableNode to use for accessor search.\n");
		return NULL;
	}
	return variable_->findAccessor(s, arglist);
}
