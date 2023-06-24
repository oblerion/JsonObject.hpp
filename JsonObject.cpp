#include "JsonObject.hpp"

const char* TextFormat(const char *__restrict __format, ...)
{
    char* s = (char*)malloc(sizeof(char)*1024);
    va_list args;
    va_start(args,__format);
    vsnprintf(s,1024,__format,args);
    va_end(args);
    return s;
}
int FileExists(const char* file)
{
    FILE* fic = fopen(file,"r");
    if(fic!=NULL)
    {
        fclose(fic);
        return 1;
    }
    return 0;
}
char* LoadFileText(const char* file)
{
    FILE* fic = fopen(file,"r");
    if(fic!=NULL)
    {
        fseek(fic,0,SEEK_END);
        int pos = ftell(fic);
        fseek(fic,0,SEEK_SET);
        char* s = (char*)malloc(sizeof(char)*pos);
        for(int i=0;i<pos;i++)
        {
            char c = fgetc(fic);
            sprintf(s,"%s%c",s,c);
        }
        fclose(fic);
    }
    return NULL;
}
JsonObject::JsonObject()
{}

JsonObject::JsonObject(std::string sfile)
{
    int in_obj = 0;
    int in_array = 0 ;
    std::vector<std::string> ldata;
    int i_tps=0;
    std::string data;
    data = _readFile(sfile);

    if(data.length()==0)
    {
        data = sfile;
    }
    else
    {
        data = _filter(data);
        if(data[0]=='{')
        {
            data = _extract(data,'{','}');
            for(unsigned int i=0;i<data.length();i++)
            {
                switch(data[i])
                {
                    case '[':
                        in_array++;
                    break;
                    case '{':
                        in_obj++;
                    break;
                    case ']':
                        in_array--;
                    break;
                    case '}':
                        in_obj--;
                    break;
                    case ',':
                        if(in_array<=0 && in_obj<=0)
                        {
                            ldata.push_back(data.substr(i_tps,i-i_tps));
                            i_tps = i+1;
                        }
                    break;
                }
                if(i==data.length()-1)
                {
                    ldata.push_back(data.substr(i_tps,i-i_tps+1));
                }
            }

            for(unsigned int i=0;i<ldata.size();i++)
            {
                std::string s2 = ldata[i];
                if(s2[0]=='"')
                {
                    std::string name = _extract(s2,'"','"');
                    std::string value = _extract(s2,':');
                    if(value!="")
                    {
                        if(value[0]=='"')
                            _addString(name,value);
                        else if(value[0]=='{')
                        {
                            //_list_obj_key
                            _addObject(name,value);
                        }
                        else if(value[0]=='[')
                        {
                            _addArray(name,value);
                        }
                        else if(value=="true" || value=="false")
                        {
                            _addBool(name,value);
                        }
                        else
                        {
                            _addInt(name,value);
                            _addFloat(name,value);
                        }
                    }
                }
            }
        }
    }
}
JsonObject::~JsonObject()
{
    _ds.clear();
    _ds_keys.clear();
    _di.clear();
    _di_keys.clear();
    _df.clear();
    _df_keys.clear();
    _db.clear();
    _db_keys.clear();
    _dj.clear();
    _dj_keys.clear();
    _dai1.clear();
    _dai1_keys.clear();
    _das1.clear();
    _das1_keys.clear();
}

void JsonObject::_addArray(std::string name,std::string value)
{
    std::string ext = _extract(value,'[',']');
    if(ext.length()==0) return;
    if(ext[0]=='"')
    {
        int in_string=0;
        std::string t_str="";
        std::vector<std::string> vstr;
        for(unsigned int i=0;i<value.length();i++)
        {
            if(value[i]=='"')
            {
                if(in_string>0)
                {
                    in_string--;
                    vstr.push_back(t_str+" ");
                    t_str="";
                }
                else in_string++;
            }
            else if(in_string==1)
            {
                t_str += value.substr(i,1);
            }
        }
        _das1.push_back(vstr);
        _das1_keys.push_back(name);
    }
    else if(ext[0]=='[')
    {}
    else if(ext[0]=='{')
    {
        int in_array=0;
        int in_obj=0;
        std::string tstr="";
        JsonObject jo = *(new JsonObject());
        for(unsigned int i=0;i<ext.length();i++)
        {
            switch(ext[i])
            {
                case '[':
                    in_array++;
                break;
                case '{':
                    in_obj++;
                break;
                case ']':
                    in_array--;
                break;
                case '}':
                    in_obj--;
                break;
                case ',':
                    if(in_array<=0 && in_obj<=0)
                    {
                        jo.SetObject(TextFormat("%d",jo.GetNbObject()),
                                     *(new JsonObject(tstr)));
                        tstr="";
                    }
                    else
                        tstr += ext.substr(i,1);
                break;
            }
            if(ext[i]!=',')
                tstr += ext.substr(i,1);
        }
        jo.SetObject(TextFormat("%d",jo.GetNbObject()),
                            *(new JsonObject(tstr)));
        _dj.push_back(jo);
        _dj_keys.push_back(name);
    }
    else
    {
        //_dai1[name]= *(new std::vector<int>());
        std::vector<int> vi;
        std::string tstr="";
        for(unsigned int i=0;i<ext.length();i++)
        {
            if(ext[i]==',')
            {
                vi.push_back(atoi(tstr.c_str()));
                tstr="";
            }
            else
            {
                tstr += ext.substr(i,1);
            }
        }
        vi.push_back(atoi(tstr.c_str()));
        _dai1.push_back(vi);
        _dai1_keys.push_back(name);
    }
}
void JsonObject::_addObject(std::string name,std::string value)
{
    //_dj[name]=*(new JsonObject(value));
    JsonObject j = *(new JsonObject(value));
    _dj.push_back(j);
    _dj_keys.push_back(name);
}
void JsonObject::_addBool(std::string name, std::string value)
{
    _db_keys.push_back(name);
    if(value=="true") _db.push_back(true);
    else _db.push_back(false);
}
void JsonObject::_addString(std::string name, std::string value)
{
    _ds_keys.push_back(name);
    _ds.push_back(_extract(value,'"','"'));
}
void JsonObject::_addInt(std::string name, std::string value)
{
    _di_keys.push_back(name);
    if(value.find('.')==std::string::npos)
        _di.push_back(atoi(value.c_str()));
}
void JsonObject::_addFloat(std::string name, std::string value)
{
    if(value!="")
    {
        _df_keys.push_back(name);
        if(value.find('.')!=std::string::npos)
        {
           // atof()
            for(unsigned int i=0;i<value.length();i++)
            {
                if(value[i]=='.')
                {
                    double o = atof(value.substr(0,i).c_str());
                    std::string ls = value.substr(i+1,value.length()-(i+1));
                    o+= atoi(ls.c_str())/pow(10,ls.length());
                    _df.push_back((float)o);
                    break;
                }
            }
        }
    }
}


std::string JsonObject::_readFile(std::string sfile)
{
    if(sfile!="" && FileExists(sfile.c_str()))
    {
        return LoadFileText(sfile.c_str());
    }
    return "";
}
void JsonObject::WriteFile(std::string sfile)
{
        //if(FileExists(sfile.c_str())){}
        std::string sdata;
        if(IsEmpty()) sdata = "{\n}";
        else sdata = ToString();
        FILE* fic;
        fic = fopen(sfile.c_str(),"w");
        if(fic!=NULL)
        {
            fprintf(fic,"%s",sdata.c_str());
            fclose(fic);
        }
}
std::string JsonObject::_filter(std::string s)
{
    std::string ls="";
    char lc;
    bool in_string = false;
    for(unsigned int i=0;i<s.length();i++)
    {
        lc = s[i];
        if(lc!=' ' &&
            lc!='\n' &&
            lc!='\b' &&
            lc!='\r' &&
            lc!='\t')
            ls+=s.substr(i,1);
        if(lc=='"')
        {
            if(in_string)
                in_string = false;
            else
                in_string = true;
        }
        if(lc==' ' && in_string)
             ls+=s.substr(i,1);
    }
    return ls;
}
std::string JsonObject::_extract(std::string s, char cbegin)
{
    int i_beg=-1;
    for(unsigned int i=0;i<s.length();i++)
    {
        if(s[i]==cbegin)
        {
            i_beg = i+1;
            break;
        }
    }
    if(i_beg==-1) return "";
    return s.substr(i_beg,s.length()-i_beg);
}

std::string JsonObject::_extract(std::string s, char cbegin, char cend)
{
    int i_beg=-1;
    int i_end=-1;
    for(unsigned int i=0;i<s.length();i++)
    {
        if(s[i]==cbegin)
        {
            i_beg=i+1;
            break;
        }
    }
    if(i_beg==-1)return "";
    for(unsigned int i=i_beg;i<s.length();i++)
    {
        if(s[i]==cend)
        {
            i_end=i;
            break;
        }
    }
    return s.substr(i_beg,i_end-i_beg);
}
std::string JsonObject::ToString()
{
    std::vector<std::string> as_key;
    std::vector<std::string> as_value;
    if(IsEmpty()) return "";

    for(unsigned int i=0;i<_ds_keys.size();i++)
    {
        as_key.push_back( TextFormat("%c%s%c",'"',_ds_keys[i].c_str() ,'"'));
        as_value.push_back(TextFormat( "%c%s%c",'"',_ds[i].c_str(),'"'));
    }
    for(unsigned int i=0;i< _di_keys.size();i++)
    {
        as_key.push_back(TextFormat("%c%s%c",'"',_di_keys[i].c_str(),'"'));
        as_value.push_back(TextFormat("%d",_di[i]));
    }
    for(unsigned int i=0;i<_df_keys.size();i++)
    {
        int iuni = (int)floor(_df[i]) ;
        int idec = (int)floor(_df[i]*10000)-iuni*10000;
        as_key.push_back(TextFormat("%c%s%c",'"',_df_keys[i].c_str(),'"'));
        as_value.push_back(TextFormat("%d.%d",iuni,idec));
    }

    for(unsigned int i=0;i<_db_keys.size();i++)
    {
        as_key.push_back( TextFormat("%c%s%c",'"',_db_keys[i].c_str(),'"'));
        if(_db[i]) as_value.push_back("true");
        else as_value.push_back("false");
    }
    for(unsigned int i=0;i<_dai1_keys.size();i++)
    {
        std::string sdata;
        sdata = "[";
        for(unsigned int j=0;j<_dai1[i].size();j++)
        {
            sdata += TextFormat(" %d",_dai1[i][j]);
            if(i<_dai1.size()) sdata += ", ";
        }
        sdata += "]";
        as_key.push_back(TextFormat("%c%s%c",'"',_dai1_keys[i].c_str(),'"'));
        as_value.push_back(sdata);
    }

    for(unsigned int i=0;i<_das1_keys.size();i++)
    {
        std::string sdata;
        sdata = "[";
        for(unsigned int j=0;j<_das1[i].size();j++)
        {
            sdata += '"' + _das1[i][j] + '"';
            if(j<_das1[i].size()-1) sdata += ", ";
        }
        sdata += "]";
        as_key.push_back(TextFormat("%c%s%c",'"',_das1_keys[i].c_str(),'"'));
        as_value.push_back(sdata);
    }
    for(unsigned int i=0;i<_dj_keys.size();i++)
    {
        as_key.push_back(TextFormat("%c%s%c",'"',_dj_keys[i].c_str(),'"'));
        as_value.push_back(_dj[i].ToString());
    }
    std::string s="{";
    for(unsigned int i=0;i<as_key.size();i++)
    {
        s += "\n" + as_key[i] + " :" + as_value[i];
        if(i<as_key.size()-1) s += ",";
    }
    s += "\n}";
    //puts(s.c_str());
    return s;
}
bool JsonObject::IsEmpty()
{
    unsigned int nb = 0;
    nb += _ds.size();
    nb += _di.size();
    nb += _df.size();
    nb += _db.size();
    nb += _das1.size();
    nb += _dai1.size();
    nb += _dj.size();
    if(0 == nb)
          return true;
    return false;
}
void JsonObject::Print()
{
    puts(ToString().c_str());
}

std::vector<int> JsonObject::GetArray(std::string key)
{
    for(unsigned int i=0;i<_dai1_keys.size();i++)
    {
        if(_dai1_keys[i]==key)
            return _dai1[i];
    }
    return *(new std::vector<int>);
}
std::vector<std::string> JsonObject::GetArrayS(std::string key)
{
    for(unsigned int i=0;i<_das1_keys.size();i++)
    {
        if(_das1_keys[i]==key)
            return _das1[i];
    }
    std::vector<std::string> vstr;
    return vstr;
}

bool JsonObject::GetBool(std::string key)
{
    for(unsigned int i=0;i<_db_keys.size();i++)
    {
        if(_db_keys[i]==key)
            return _db[i];
    }
    return false;
}
std::string JsonObject::GetString(std::string key)
{
    for(unsigned int i=0;i<_ds_keys.size();i++)
    {
        if(_ds_keys[i]==key)
            return _ds[i];
    }
    return "";
}
int JsonObject::GetInt(std::string key)
{
    for(unsigned int i=0;i<_di_keys.size();i++)
    {
        if(_di_keys[i]==key)
            return _di[i];
    }
    return -1;
}
float JsonObject::GetFloat(std::string key)
{
    for(unsigned int i=0;i<_df_keys.size();i++)
    {
        if(_df_keys[i]==key)
            return _df[i];
    }
    return -1;
}
JsonObject JsonObject::GetObject(std::string key)
{
    for(unsigned int i=0;i<_dj_keys.size();i++)
    {
        if(_dj_keys[i]==key)
            return _dj[i];
    }
    return *(new JsonObject());
}

JsonObject JsonObject::GetObject(int id)
{
    if(id>-1 && id<(signed int)_dj.size())
    {
        return _dj[id];
    }
    return *(new JsonObject());
}
int JsonObject::GetNbObject()
{
    return _dj.size();
}
void JsonObject::SetArray(std::string key, std::vector<int> value)
{
    for(unsigned int i=0;i<_dai1_keys.size();i++)
    {
        if(_dai1_keys[i]==key)
        {
            _dai1[i] = value;
            return;
        }
    }
    // not exist create
	_dai1_keys.push_back(key);
	_dai1.push_back(value);
	
}
void JsonObject::SetArray(std::string key, std::vector<std::string> value)
{
    for(unsigned int i=0;i<_das1_keys.size();i++)
    {
        if(_das1_keys[i]==key)
        {
            _das1[i]=value;
            return;
        }
    }
    
	_das1_keys.push_back(key);
	_das1.push_back(value);
}

void JsonObject::SetObject(std::string key, JsonObject value)
{
    for(unsigned int i=0;i<_dj_keys.size();i++)
    {
        if(_dj_keys[i]==key){
            _dj[i] = *(new JsonObject(value.ToString()));
            return;
        }
    }
	_dj_keys.push_back(key);
	_dj.push_back( *(new JsonObject(value.ToString())));

}
void JsonObject::SetBool(std::string key, bool value)
{
    for(unsigned int i=0;i<_db_keys.size();i++)
    {
        if(_db_keys[i]==key){
            _db[i] = value;
            return;
        }
    }
    _db_keys.push_back(key);
    _db.push_back(value);
}
void JsonObject::SetString(std::string key, std::string value)
{
    for(unsigned int i=0;i<_ds_keys.size();i++)
    {
        if(_ds_keys[i]==key)
        {
            _ds[i]=value;
            return;
        }
    }
    _ds_keys.push_back(key);
    _ds.push_back(value);
}
void JsonObject::SetInt(std::string key, int value)
{
    for(unsigned int i=0;i<_di_keys.size();i++)
    {
        if(_di_keys[i]==key)
        {
            _di[i]=value;
            return;
        }
    }
    _di_keys.push_back(key);
    _di.push_back(value);
}
void JsonObject::SetFloat(std::string key, float value)
{
    for(unsigned int i=0;i<_df_keys.size();i++)
    {
        if(_df_keys[i]==key)
        {
            _df[i]=value;
			return;
        }
    }
    _df_keys.push_back(key);
    _df.push_back(value);
    
}
void JsonObject::DelArray(std::string key)
{
    for(unsigned int i=0;i<_dai1_keys.size();i++)
    {
        if(_dai1_keys[i]==key)
        {
            _dai1.erase(_dai1.begin()+i);
            _dai1_keys.erase(_dai1_keys.begin()+i);
            return;
        }
    }
    for(unsigned int i=0;i<_das1_keys.size();i++)
    {
        if(_das1_keys[i]==key)
        {
            _das1.erase(_das1.begin()+i);
            _das1_keys.erase(_das1_keys.begin()+i);
            return;
        }
    }
}
void JsonObject::DelObject(std::string key)
{
    for(unsigned int i=0;i<_dj_keys.size();i++)
    {
        if(_dj_keys[i]==key)
        {
            _dj.erase(_dj.begin()+i);
            _dj_keys.erase(_dj_keys.begin()+i);
            return;
        }

    }
}
void JsonObject::DelBool(std::string key)
{
    for(unsigned int i=0;i<_db_keys.size();i++)
    {
        if(_db_keys[i]==key)
        {
            _db.erase(_db.begin()+i);
            _db_keys.erase(_db_keys.begin()+i);
            return;
        }
    }
}
void JsonObject::DelString(std::string key)
{
    for(unsigned int i=0;i<_ds_keys.size();i++)
    {
        if(_ds_keys[i]==key)
        {
            _ds.erase(_ds.begin()+i);
            _ds_keys.erase(_ds_keys.begin()+i);
            return;
        }
    }
}
void JsonObject::DelInt(std::string key)
{
    for(unsigned int i=0;i<_di_keys.size();i++)
    {
        if(_di_keys[i]==key)
        {
            _di.erase(_di.begin()+i);
            _di_keys.erase(_di_keys.begin()+i);
            return;
        }
    }
}


