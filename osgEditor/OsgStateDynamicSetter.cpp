#include "StdAfx.h"
#include "OsgStateDynamicSetter.h"


//************************************
// Method:    apply
// FullName:  COsgStateDynamicSetter::apply
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: osg::Geode & geode
//************************************
void COsgStateDynamicSetter::apply(osg::Geode& geode)
{
	int num = geode.getNumDrawables();

	for (int j = num - 1; j >= 0; --j)
	{
		apply(geode.getDrawable(j)->getStateSet());
	}
}

//************************************
// Method:    apply
// FullName:  COsgStateDynamicSetter::apply
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: osg::StateSet * state
//************************************
void COsgStateDynamicSetter::apply(osg::StateSet* state)
{
	if (!state)
	{
		return;
	}

	osg::StateSet::TextureAttributeList& texAttribList = state->getTextureAttributeList();

	for(unsigned int i = 0; i < texAttribList.size(); i++)
	{
		osg::Texture2D* tex2D = dynamic_cast<osg::Texture2D*>(state->getTextureAttribute(i, osg::StateAttribute::TEXTURE));

		if(tex2D)
		{
			tex2D->setDataVariance(osg::Object::DataVariance::DYNAMIC);
			int imageNum = tex2D->getNumImages();

			for (int i = 0; i < imageNum; i++)
			{
				osg::ref_ptr<osg::Image> image = tex2D->getImage();
				
				if (!image)
				{
					continue;
				}

				image->setDataVariance(osg::Object::DYNAMIC);
			}

		}
	}
}