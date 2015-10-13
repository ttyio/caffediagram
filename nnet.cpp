#include  <stdio.h>
#include  <string.h>
#include  "nnet.h"

Layer::Layer()
    :m_next(NULL)
{
}

Layer::~Layer()
{
    while (m_next){
        Layer* pNext = m_next->m_next;
        delete m_next;
        m_next = pNext;
    }
}

NNet::NNet()
    :m_curLayer(NULL)
{
}

NNet::~NNet()
{
    if (m_curLayer){
        delete m_curLayer;
        m_curLayer = NULL;
    }
    std::map<std::string, Layer*>::const_iterator iter;
    for (iter=m_layers.begin(); iter!=m_layers.end(); ++iter){
        delete iter->second;
    }
    m_layers.clear();
    m_globalAttribs.clear();
}

bool NNet::beginLayer()
{
    printf("Begin layer\n");
    m_curLayer = new Layer();
    return true;
}

bool NNet::endLayer()
{
    printf("End layer\n");
    if (m_curLayer){
        if (!m_curLayer->m_name.empty()){
            if (findbottom(m_curLayer->m_name.c_str())){
            }
            m_layers[m_curLayer->m_name] = m_curLayer;
        } else {
            delete m_curLayer;
        }
        m_curLayer = NULL;
    }
    return true;
}

bool NNet::setAttri(int level, const char* curAttriBlock, const char* attri, const char* value)
{
    if (!attri || !value){
        return false;
    }
    const char* attritrim = attri;
    while (*attritrim == ' '){
        ++attritrim;
    }
    const char* valuetrim = value;
    while (*valuetrim == ' '){
        ++valuetrim;
    }

    switch (level){
    case 0:
        onSetGlobalAttri(attritrim, valuetrim);
        break;
    case 1:
        onSetLayerAttri(attritrim, valuetrim);
        break;
    default:
        if (!curAttriBlock){
            return false;
        } else {
            const char* curAttriBlockTrim = curAttriBlock;
            while (*curAttriBlockTrim == ' '){
                ++curAttriBlockTrim;
            }
            onSetLayerParamAttri(curAttriBlock, attritrim, valuetrim);
        }
        break;
    }
    return true;
}

void NNet::onSetGlobalAttri(const char* attri, const char* value)
{
    printf("Net attri(%s = \"%s\")", attri, value);
    m_globalAttribs[attri] = value;
}


void NNet::onSetLayerAttri(const char* attri, const char* value)
{
    #define CASE_SINGLE_ATTRI(_NAME)                        \
        } else if (0 == strcmp(attri, #_NAME)){             \
            m_curLayer->m_##_NAME = value;                  \

    #define CASE_MULTI_ATTRI(_NAME)                         \
        } else if (0 == strcmp(attri, #_NAME)){             \
            m_curLayer->m_##_NAME.insert(value);            \

    printf("Layer attri(%s = \"%s\")", attri, value);

    if (m_curLayer){
        if (0){
            LAYER_SINGLE_ATTRI_LIST(CASE_SINGLE_ATTRI)
            LAYER_MULTI_ATTRI_LIST(CASE_MULTI_ATTRI)
        } else {
            m_curLayer->m_attribs[attri] = value;
        }
    }

    #undef CASE_SINGLE_ATTRI
    #undef CASE_MULTI_ATTRI
}

void NNet::onSetLayerParamAttri(const char* curParamGroup, const char* attri, const char* value)
{
    printf("Layer param(%s, %s = \"%s\")", curParamGroup, attri, value);
    if (!m_curLayer){
        return;
    }

    if (0 == strcmp(curParamGroup, "include")){
        if (0 == strcmp(attri, "phase")){
            if (strcmp(attri, "TRAIN")){
                delete m_curLayer;
                m_curLayer = NULL;
            }
        }
    }
}

#define DEF_MULTI_FIND(_NAME)                                       \
Layer* NNet::find##_NAME(const char* name)                          \
{                                                                   \
    std::map<std::string, Layer*>::iterator iter;                   \
    for (iter=m_layers.begin(); iter!=m_layers.end(); ++iter){      \
        Layer* pLayer = iter->second;                               \
        if (!pLayer){                                               \
            continue;                                               \
        }                                                           \
        if (pLayer->m_##_NAME.find(name)!=pLayer->m_##_NAME.end()){ \
            return pLayer;                                          \
        }                                                           \
    }                                                               \
    return NULL;                                                    \
}                                                                   \

LAYER_MULTI_ATTRI_LIST(DEF_MULTI_FIND);

#undef DEF_MULTI_FIND

