#ifndef __N_NET_H__
#define __N_NET_H__

#include <string>
#include <set>
#include <map>

#define LAYER_SINGLE_ATTRI_LIST(_MACRO)         \
    _MACRO(name)                                \
    _MACRO(type)                                \

#define LAYER_MULTI_ATTRI_LIST(_MACRO)          \
    _MACRO(top)                                 \
    _MACRO(bottom)                              \

#define DEF_SINGLE_ATTRI(_NAME)                 \
    std::string                             m_##_NAME;

#define DEF_MULTI_ATTRI(_NAME)                  \
    std::set<std::string>                   m_##_NAME;

#define DECLARE_MULTI_FIND(_NAME)                             \
Layer* find##_NAME(const char* name);                         \


struct Layer{
    Layer();
    ~Layer();

    LAYER_SINGLE_ATTRI_LIST(DEF_SINGLE_ATTRI);
    LAYER_MULTI_ATTRI_LIST(DEF_MULTI_ATTRI);

    unsigned int w();
    unsigned int h();
    unsigned int c();

    std::map<std::string, std::string>      m_attribs;
    Layer* m_next;
};


class NNet{
public:
    NNet();
    ~NNet();

    bool beginLayer();
    bool endLayer();
    bool setAttri(int level, const char* curAttriBlock, const char* attri, const char* value);

    void draw(const char* fname);

protected:
    void onSetGlobalAttri(const char* attri, const char* value);
    void onSetLayerAttri(const char* attri, const char* value);
    void onSetLayerParamAttri(const char* curParamGroup, const char* attri, const char* value);

private:
    LAYER_MULTI_ATTRI_LIST(DECLARE_MULTI_FIND);

private:
    Layer*                                  m_curLayer;
    std::map<std::string, Layer*>           m_layers;
    std::map<std::string, std::string>      m_globalAttribs;
};


#endif
