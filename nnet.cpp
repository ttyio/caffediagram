#include  <stdio.h>
#include  <string.h>
#include  <set>
#include  <assert.h>
#include  <stdlib.h>
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

bool Layer::calcInputDim(unsigned& width, unsigned& height, unsigned& channel)
{
    std::map<std::string, std::map<std::string, std::string> >::const_iterator citer = m_paramAttribs.find("convolution_param");
    if (citer != m_paramAttribs.end()){
        std::map<std::string, std::string>::const_iterator attriIter = citer->second.find("num_output");
        if (attriIter != citer->second.end()){
            channel = atoi(attriIter->second.c_str());
        }
        attriIter = citer->second.find("kernel_size");
        if (attriIter != citer->second.end()){
            if (strcmp(m_type.c_str(), "CONVOLUTION") == 0){
                width -= atoi(attriIter->second.c_str())/2*2;
                height -= atoi(attriIter->second.c_str())/2*2;
            } else if (strcmp(m_type.c_str(), "DECONVOLUTION") == 0){
                width += atoi(attriIter->second.c_str())/2*2;
                height += atoi(attriIter->second.c_str())/2*2;
            }
        }
        attriIter = citer->second.find("pad");
        if (attriIter != citer->second.end()){
            if (strcmp(m_type.c_str(), "CONVOLUTION") == 0){
                width += atoi(attriIter->second.c_str())*2;
                height += atoi(attriIter->second.c_str())*2;
            } else if (strcmp(m_type.c_str(), "DECONVOLUTION") == 0){
                width -= atoi(attriIter->second.c_str())*2;
                height -= atoi(attriIter->second.c_str())*2;
            }
        }

    }

    citer = m_paramAttribs.find("pooling_param");
    if (citer != m_paramAttribs.end()){
        std::map<std::string, std::string>::const_iterator attriIter = citer->second.find("kernel_size");
        if (attriIter != citer->second.end()){
            width /= atoi(attriIter->second.c_str());
            height /= atoi(attriIter->second.c_str());
        }
    }

    citer = m_paramAttribs.find("unpooling_param");
    if (citer != m_paramAttribs.end()){
        std::map<std::string, std::string>::const_iterator attriIter = citer->second.find("kernel_size");
        if (attriIter != citer->second.end()){
            width *= atoi(attriIter->second.c_str());
            height *= atoi(attriIter->second.c_str());
        }
    }
}

NNet::NNet(unsigned nWidth, unsigned nHeight, unsigned nChannel)
    :m_curLayer(NULL)
{
    g_inputWidth = nWidth;
    g_inputHeight = nHeight;
    g_inputChannel = nChannel;
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

    if (m_curLayer){
        std::map<std::string, std::map<std::string, std::string> >::iterator iter = m_curLayer->m_paramAttribs.find(curParamGroup);
        if (iter == m_curLayer->m_paramAttribs.end()){
            std::map<std::string, std::string> tmpMap;
            tmpMap[attri] = value;
            m_curLayer->m_paramAttribs.insert(make_pair((std::string)curParamGroup, tmpMap));
        } else {
            iter->second[attri] = value;
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
    int nCanvasWidth = 0;
    int nCanvasHeight = 0;
    draw(fname, nCanvasWidth, nCanvasHeight);
    draw(fname, nCanvasWidth, nCanvasHeight);
}

void NNet::draw(const char* fname, int& nCanvasWidth, int& nCanvasHeight)
{
    NetWritter writter;
    int nTotal = m_layers.size();
    int nElementHeight = LAYER_HEIGHT*1.5;
    int nElementWidth = LAYER_WIDTH*1.1;
    if (!nCanvasWidth){
        nCanvasWidth = nTotal * (nElementWidth+LAYER_MARGIN_HOR);
    }
    if (!nCanvasHeight){ 
        nCanvasHeight = MAX_LAYER_PER_COL*(nElementHeight+LAYER_MARGIN_VER*2);
    }
    writter.create(nCanvasWidth, nCanvasHeight, BmpWritter::WHITE);
    int nPosX = LAYER_MARGIN_HOR;
    int nPosY = LAYER_MARGIN_VER;

    unsigned maxHoriLayer = 0;
    unsigned maxVertLayer = 0;
    unsigned nVertLayer = 0;

    unsigned width = g_inputWidth;
    unsigned height = g_inputHeight;
    unsigned channel = g_inputChannel;
    std::set<std::string> completeLayers;
    // draw input
    for (std::map<std::string, Layer*>::iterator iter = m_layers.begin(); iter!=m_layers.end(); ++iter){
        Layer* layer = iter->second;
        assert(layer);
        if (layer->m_bottom.empty()){
            printf("draw %s\n", layer->m_name.c_str());
            layer->calcInputDim(width, height, channel);
            writter.drawLayer(layer->m_name.c_str(), nPosX, nPosY, LAYER_WIDTH, LAYER_HEIGHT, width, height, channel, BmpWritter::BLACK);
            nPosY += nElementHeight;
            completeLayers.insert(layer->m_name);
            ++nVertLayer;
        }
    }
    maxVertLayer = maxVertLayer>nVertLayer?maxVertLayer:nVertLayer;
    nVertLayer = 0;
    ++maxHoriLayer;

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

            layer->calcInputDim(width, height, channel);
            writter.drawLayer(name.c_str(), nPosX, nPosY, LAYER_WIDTH, LAYER_HEIGHT, width, height, channel, BmpWritter::BLACK);
            namesThisLayer.insert(layer->m_name);
            namesThisLayer.insert(layer->m_top.begin(), layer->m_top.end());
            ++nVertLayer;
            nPosY += nElementHeight;
        }

        maxVertLayer = maxVertLayer>nVertLayer?maxVertLayer:nVertLayer;
        nVertLayer = 0;
        ++maxHoriLayer;

        completeLayers.insert(namesThisLayer.begin(), namesThisLayer.end());
        if (completeLayers.size() == nLastCount){
            break;
        }
        nLastCount = completeLayers.size();
    }

    nCanvasWidth = maxHoriLayer * (nElementWidth+LAYER_MARGIN_HOR);
    nCanvasHeight = maxVertLayer * (nElementHeight+LAYER_MARGIN_VER*2);
    
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
