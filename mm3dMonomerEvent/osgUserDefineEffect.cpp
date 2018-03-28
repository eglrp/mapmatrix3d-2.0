#include "stdafx.h"
#include "osgUserDefineEffect.h"
#include <osgFX/Technique>
#include <osg/Polygonoffset>
#include <osg/PolygonMode>
#include <osg/Texture2D>

class CUserDefineTechnique : public osgFX::Technique 
{
public:
	CUserDefineTechnique(osg::Material* wf_mat, osg::LineWidth* wf_lw)
		: Technique(), _wf_mat(wf_mat), _wf_lw(wf_lw) {}

	bool validate(osg::State&) const
	{
		return strncmp((const char*)glGetString(GL_VERSION), "1.1", 3) >= 0;
	}

protected:

	void define_passes()
	{
		// implement pass #1
		{
			osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;

			osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
			polyoffset->setFactor(1.0f);
			polyoffset->setUnits(1.0f);
			ss->setAttributeAndModes(polyoffset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

			addPass(ss.get());
		}

		// implement pass #2
		{
			osg::ref_ptr<osg::StateSet> ss = new osg::StateSet;

			osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
			polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
			ss->setAttributeAndModes(polymode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
			ss->setAttributeAndModes(_wf_lw.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
			ss->setAttributeAndModes(_wf_mat.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
			ss->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
			ss->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
			addPass(ss.get());
		}
	}

private:
	osg::ref_ptr<osg::Material> _wf_mat;
	osg::ref_ptr<osg::LineWidth> _wf_lw;
};


bool COsgUserDefineEffect::define_techniques()
{
	addTechnique(new CUserDefineTechnique(_wf_mat.get(), _wf_lw.get()));
	return true;
}

