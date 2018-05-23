#pragma once
#include <windows.h>
#include <osgFX/Effect>
#include <osg/Material>
#include <osg/LineWidth>

class COsgUserDefineEffect : public osgFX::Effect
{
public:
	COsgUserDefineEffect() : osgFX::Effect(), _wf_mat(new osg::Material), _wf_lw(new osg::LineWidth)
	{
		_wf_lw->setWidth(1.0f);
		_wf_mat->setColorMode(osg::Material::EMISSION);
		_wf_mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
		_wf_mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
		_wf_mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
		_wf_mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	}

	/** get the wireframe color */
	inline const osg::Vec4& getWireframeColor() const;

	/** set the wireframe color */
	inline void setWireframeColor(const osg::Vec4& color);

	/** get the wireframe line width */
	inline float getWireframeLineWidth() const;

	/** set the wireframe line width */
	inline void setWireframeLineWidth(float w);

	/** get the name of this Effect */
	virtual const char *effectName() const { return NULL; }

	/** get a brief description of this Effect*/
	virtual const char *effectDescription() const { return NULL; }

	/** get the effect author's name */
	virtual const char *effectAuthor() const {return NULL;}

protected:
	virtual ~COsgUserDefineEffect() {}
	COsgUserDefineEffect& operator=(const COsgUserDefineEffect&) { return *this; }

	virtual bool define_techniques();

private:
	osg::ref_ptr<osg::Material> _wf_mat;
	osg::ref_ptr<osg::LineWidth> _wf_lw;
};


inline const osg::Vec4& COsgUserDefineEffect::getWireframeColor() const
{
	return _wf_mat->getEmission(osg::Material::FRONT_AND_BACK);
}

inline void COsgUserDefineEffect::setWireframeColor(const osg::Vec4& color)
{
	_wf_mat->setEmission(osg::Material::FRONT_AND_BACK, color);
}

inline float COsgUserDefineEffect::getWireframeLineWidth() const
{
	return _wf_lw->getWidth();
}

inline void COsgUserDefineEffect::setWireframeLineWidth(float w)
{
	_wf_lw->setWidth(w);
}