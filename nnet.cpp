#include  <stdio.h>
#include  <string.h>
#include  <set>
#include  <assert.h>
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

unsigned int Layer::w()
{
    return 0;
}

unsigned int Layer::h()
{
    return 0;
}

unsigned int Layer::c()
{
    return 0;
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
            assert(m_layers.find(m_curLayer->m_name) == m_layers.end());
            m_layers[m_curLayer->m_name] = m_curLayer;
            printf("Layer %s saved.\n", m_curLayer->m_name.c_str());
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
    printf("Net attri(%s = \"%s\")\n", attri, value);
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

    printf("Layer attri(%s = \"%s\")\n", attri, value);

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
    printf("Layer param(%s, %s = \"%s\")\n", curParamGroup, attri, value);
    if (!m_curLayer){
        return;
    }

    if (0 == strcmp(curParamGroup, "include")){
        if (0 == strcmp(attri, "phase")){
            if (strcmp(value, "TRAIN")){
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

#include  "netwritter.h"
#define LAYER_WIDTH     80
#define LAYER_HEIGHT    80
#define LAYER_MARGIN_VER    20
#define LAYER_MARGIN_HOR    20
#define MAX_LAYER_PER_COL   2

void NNet::draw(const char* fname)
{
    NetWritter writter;
    int nTotal = m_layers.size();
    int nElementHeight = LAYER_HEIGHT*1.5;
    int nElementWidth = LAYER_WIDTH*1.1;
    int nCanvasWidth = nTotal * (nElementWidth+LAYER_MARGIN_HOR);
    int nCanvasHeight = MAX_LAYER_PER_COL*(nElementHeight+LAYER_MARGIN_VER*2);
    writter.create(nCanvasWidth, nCanvasHeight, BmpWritter::WHITE);
    int nPosX = LAYER_MARGIN_HOR;
    int nPosY = LAYER_MARGIN_VER;

    std::set<std::string> completeLayers;
    // draw input
    for (std::map<std::string, Layer*>::iterator iter = m_layers.begin(); iter!=m_layers.end(); ++iter){
        Layer* layer = iter->second;
        assert(layer);
        if (layer->m_bottom.empty()){
            printf("draw %s\n", layer->m_name.c_str());
            writter.drawLayer(layer->m_name.c_str(), nPosX, nPosY, LAYER_WIDTH, LAYER_HEIGHT, 0, 0, 0, BmpWritter::BLACK);
            nPosY += nElementHeight;
            completeLayers.insert(layer->m_name);
        }
    }

    // draw successor
    int nLastCount = 0;
    while (1){
        // init pos
        nPosX += (nElementWidth+LAYER_MARGIN_HOR);
        nPosY = LAYER_MARGIN_VER;

        // check other layers can be combined with current layer
        std::set<std::string> namesThisLayer;

        for (std::map<std::string, Layer*>::iterator iter = m_layers.begin(); iter!=m_layers.end(); ++iter){
            Layer* layer = iter->second;
            assert(layer);
            if (completeLayers.find(layer->m_name) != completeLayers.end()){
                continue;
            }

            // check if current layer's bottom has finished drawing
            bool allBottomDraw = true;
            for (std::set<std::string>::const_iterator bottomIter=layer->m_bottom.begin(); bottomIter!=layer->m_bottom.end();++bottomIter){
                const std::string& strBottom = *bottomIter;
                if (completeLayers.find(strBottom) == completeLayers.end()){
                    allBottomDraw = false;
                    break;
                }
            }
            if (!allBottomDraw){
                continue;
            }

            std::string name = layer->m_name;
            for (std::map<std::string, Layer*>::iterator iter = m_layers.begin(); iter!=m_layers.end(); ++iter){
                Layer* layer2nd = iter->second;
                assert(layer2nd);
                if (completeLayers.find(layer->m_name) != completeLayers.end() ||
                    layer2nd == layer){
                    continue;
                }

                if (layer2nd->m_top.find(layer->m_name) != layer2nd->m_top.end()){
                    name += ",";
                    name += layer2nd->m_name;
                    namesThisLayer.insert(layer2nd->m_name);
                    namesThisLayer.insert(layer2nd->m_top.begin(), layer2nd->m_top.end());
                }
            }

            writter.drawLayer(name.c_str(), nPosX, nPosY, LAYER_WIDTH, LAYER_HEIGHT, layer->w(), layer->h(), layer->c(), BmpWritter::BLACK);
            namesThisLayer.insert(layer->m_name);
            namesThisLayer.insert(layer->m_top.begin(), layer->m_top.end());
            nPosY += nElementHeight;
        }
        completeLayers.insert(namesThisLayer.begin(), namesThisLayer.end());
        if (completeLayers.size() == nLastCount){
            break;
        }
        nLastCount = completeLayers.size();
    }
    printf("Complete names:\n");
    for (std::map<std::string, Layer*>::iterator iter = m_layers.begin(); iter!=m_layers.end(); ++iter){
        Layer* layer = iter->second;
        assert(layer);
        if (completeLayers.find(layer->m_name) != completeLayers.end()){
            printf("\t\t%s\n", layer->m_name.c_str());
        }
    }
    printf("Incomplete names:\n");
    for (std::map<std::string, Layer*>::iterator iter = m_layers.begin(); iter!=m_layers.end(); ++iter){
        Layer* layer = iter->second;
        assert(layer);
        if (completeLayers.find(layer->m_name) == completeLayers.end()){
            printf("\t\t%s\n", layer->m_name.c_str());
        }
    }

    writter.saveImage(fname);
}
