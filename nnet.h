#ifndef __N_NET_H__
#define __N_NET_H__

#include <string>
#include <set>
#include <map>

struct Layer{
    std::string                             m_type;
    std::string                             m_name;
    std::set<std::string>                   m_tops;
    std::set<std::string>                   m_bottoms;
    std::map<std::string, std::string>      m_attribs;
};


class NNet{
public:
    NNet();
    ~NNet();

    bool beginLayer();
    bool endLayer();
    bool setAttri(int level, const char* curAttriBlock, const char* attri, const char* value);

private:
    Layer*                                  m_curLayer;
    std::map<std::string, Layer*>           m_layers;
    std::map<std::string, std::string>      m_attribs;
};


#endif
