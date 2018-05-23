#include "stdafx.h"
#include "qcomm.h"
#include "ThreadSafeUpdateCallback.h"


CThreadSafeUpdateCallback::CThreadSafeUpdateCallback(void)
{
}


CThreadSafeUpdateCallback::~CThreadSafeUpdateCallback(void)
{
}

void CThreadSafeUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	// note, callback is responsible for scenegraph traversal so
	// they must call traverse(node,nv) to ensure that the
	// scene graph subtree (and associated callbacks) are traversed.
	osg::ref_ptr<osg::Group> spGroup = node->asGroup();
	if( spGroup!=NULL )
	{
		long key = (long)node;
		osg::ref_ptr<osg::Group> spChilds = NULL;

		//////////////////////////////////////////////////////////////////////////
		// Add Child
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_addMutex);
			MAP_NODE::iterator it = m_mapAddChilds.find(key);
			if( it!=m_mapAddChilds.end() )
			{
				spChilds = it->second;
				m_mapAddChilds.erase(it);		
			}
		}
		
		if( spChilds.valid() )
		{
			for( int i=0; i<spChilds->getNumChildren(); i++ )
			{
				osg::ref_ptr<osg::Node> spNode = spChilds->getChild(i);
				if( spNode==NULL )
					continue;

				spGroup->addChild(spNode.get());
			}

			spChilds = NULL;
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Remove Child
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_removeMutex);
			MAP_NODE::iterator it = m_mapRemoveChilds.find(key);
			if( it!=m_mapRemoveChilds.end() )
			{
				spChilds = it->second;
				m_mapRemoveChilds.erase(it);		
			}
		}

		if( spChilds.valid() )
		{
			for( int i=0; i<spChilds->getNumChildren(); i++ )
			{
				osg::ref_ptr<osg::Node> spNode = spChilds->getChild(i);
				if( spNode==NULL )
					continue;

				spGroup->removeChild(spNode.get());
			}

			spChilds = NULL;
		}
		//////////////////////////////////////////////////////////////////////////
	}
	
	traverse(node,nv);
}

bool CThreadSafeUpdateCallback::hasThreadSafeUpdateCallback(osg::Callback* pCallback)
{
	if (pCallback == NULL)
		return false;

	osg::ref_ptr<osg::NodeCallback> spNodeCallback = dynamic_cast<osg::NodeCallback*>(pCallback);
	if (!spNodeCallback.valid())
		return false;

	osg::ref_ptr<CThreadSafeUpdateCallback> spThreadSafeUpdateCallback = dynamic_cast<CThreadSafeUpdateCallback*>(spNodeCallback.get());
	if (spThreadSafeUpdateCallback.valid())
		return true;

	return hasThreadSafeUpdateCallback(spNodeCallback->getNestedCallback());
}

bool CThreadSafeUpdateCallback::AddChild(osg::Node* parent, osg::Node* node)
{
	if( parent==NULL || node==NULL )
		return false;

	long key = (long)parent;
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_addMutex);	 
	MAP_NODE::iterator it = m_mapAddChilds.find(key);
	osg::ref_ptr<osg::Group> spGroup = NULL;
	if( it!=m_mapAddChilds.end() )
		spGroup = it->second;
	else
	{
		spGroup = new osg::Group();
		m_mapAddChilds[key] = spGroup;
		if (!hasThreadSafeUpdateCallback(parent->getUpdateCallback()))
			parent->addUpdateCallback(this);
	}

	if( spGroup->containsNode(node) )
		return false;

	spGroup->addChild(node);
	return true;
}

bool CThreadSafeUpdateCallback::RemoveChild(osg::Node* parent, osg::Node* node)
{
	if( parent==NULL || node==NULL )
		return false;

	long key = (long)parent;
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_addMutex);	 
		MAP_NODE::iterator it = m_mapAddChilds.find(key);		 
		if( it!=m_mapAddChilds.end() )
		{
			osg::ref_ptr<osg::Group> spGroup = it->second;
			if( spGroup.valid() )			
				spGroup->removeChild(node);			
		}
	}

	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_removeMutex);	 
	MAP_NODE::iterator it = m_mapRemoveChilds.find(key);
	osg::ref_ptr<osg::Group> spGroup = NULL;
	if( it!=m_mapRemoveChilds.end() )
		spGroup = it->second;
	else
	{
		spGroup = new osg::Group();
		m_mapRemoveChilds[key] = spGroup;
		if (!hasThreadSafeUpdateCallback(parent->getUpdateCallback()))
			parent->addUpdateCallback(this);
	}

	if( spGroup->containsNode(node) )
		return false;

	spGroup->addChild(node);
	return true;
}
