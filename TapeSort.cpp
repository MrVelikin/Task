#include <iostream>
#include <fstream>
#include <cstdio>
#include <functional>
#include <string>
#include <io.h>
#include <algorithm>
#include <cstring>
#include <queue>

using namespace std;

struct MinHeapNode
{
    int element;
    int i;
};
 
struct comp
{
    bool operator()(const MinHeapNode &lhs, const MinHeapNode &rhs) const {
        return lhs.element > rhs.element;
    }
};

class Config
{
    ifstream conf;
    int delay_get;
    int delay_set;
    int delay_seek;
    int RAM_size=0;
public:
    Config(string file) 
    {
        conf.open(file);
        conf>> delay_get>> delay_set>> delay_seek>> RAM_size;
        conf.close();
    }

    int DelaeyGet(){
        return delay_get;
    }

    int DelaeySet(){
        return delay_set;
    }

    int Ram_Size(){
        if(RAM_size!=0)
            return RAM_size;
        else
            return 1000;
    }
};

struct IReadonlyTape
{
    virtual ~IReadonlyTape() { }

    virtual int Get() const = 0;
    virtual void Next() = 0;
    virtual void Prev() = 0;
    virtual int SizeTape() = 0;
    virtual int RewindTape() = 0;
};

struct ITape : public virtual IReadonlyTape
{
    ~ITape() override { }

    virtual void Set(int value) = 0;
};

class FileTape : public virtual ITape
{
private:
    FILE *_file;


public:
    FileTape(const char file[],const char format[]): _file(fopen(file,format)) 
    {
        
        if(_file==NULL) 
        {
            printf ("Cannot open file.\n");
            exit(1);
        } 
    }
    ~FileTape(){fclose(_file);}

    int32_t Get() const override {
        int value = 0;
        //char* buf = (char*)&value;
        fread(&value, sizeof(value), 1, _file);
        fseek( _file ,  -4 , SEEK_CUR );
        return value;
    }

    void Next() override {
        int value = 0;
        fseek( _file ,  4 , SEEK_CUR );
        if (  fread(&value, sizeof(value), 1, _file)!=1)
            {fseek( _file ,  -4 , SEEK_CUR );
                cout<<"Right end of tape"<<endl;}
        else
            fseek( _file ,  -4 , SEEK_CUR );


     }
    int SizeTape() override {
        int pos=ftell(_file);
        int sizeb=0;
        fseek(_file, 0L, SEEK_END);
        sizeb = ftell(_file);
        rewind(_file);
        fseek( _file , pos , SEEK_CUR );
        return sizeb/4;
     } 

    void Prev() override {

        if (fseek( _file ,  -4 , SEEK_CUR )!=0)
            cout<<"Left end of tape"<<endl;

     }

     int RewindTape() override {
        int CountStep=0;
        while (1)
        {
            if (fseek( _file ,  -4 , SEEK_CUR )!=0)
                break;
            else 
                CountStep++;
        }
           return CountStep;
     }

    void Set(int32_t value) override {
        fwrite(&value, sizeof(value), 1, _file);
        fseek( _file ,  -4 , SEEK_CUR );
    }
    friend void TapeSort(FileTape tape, int RAM,const char outTape[]);
    friend void mergeTapes(const char* output_Tape, int n, int Count);
};

void mergeTapes(const char* output_Tape, int n, int Count)
{
    FILE* VremTape[Count];

    for (int i = 0; i < Count; i++)
    {
        char fileName[2];
        snprintf(fileName, sizeof(fileName), "%d", i);
        VremTape[i] = fopen(fileName, "rb");
    }
    FileTape out(output_Tape,"wb");

    MinHeapNode harr[Count];
    priority_queue<MinHeapNode, vector<MinHeapNode>, comp> pq;
 
    int i;
    for (i = 0; i < Count; i++)
    {      
        if (fread(&harr[i].element, sizeof(harr[i].element), 1, VremTape[i]) != 1) {
            break;
        }
        harr[i].i = i;
        pq.push(harr[i]);
    }
 
    int count = 0;

    while (count != i)
    {
        MinHeapNode root = pq.top();
        pq.pop();
        out.Set(root.element);
        fseek( out._file ,  4, SEEK_CUR );
        if (fread(&root.element, sizeof(harr[i].element), 1, VremTape[root.i]) != 1)
        {
            root.element = numeric_limits<int>::max();
            count++;
        }
        pq.push(root);
    }
 
    for (int i = 0; i < Count; i++) {
        fclose(VremTape[i]);
        char fileName[2];
        snprintf(fileName, sizeof(fileName), "%d", i);
        if(remove(fileName)) {
            printf("Error removing file");
        }
    }
}
void TapeSort(FileTape tape, int RAM,const char outTape[])
{
    int value = 0;
    int N=tape.SizeTape();
    char fileName[2];
    int Mass[RAM]{};
    int CountOfVremTape=N/RAM+ (N%RAM ? 1 : 0);
    FILE* VremTape[CountOfVremTape];
    tape.RewindTape();
    int i=0;
    int j=0;

    //Разбиение входной ленты на отсортированные временные ленты(в зависимости от доступной ОЗУ(указывается в config.txt) или на ленты длиной 1000(если доступное ОЗУ не указано))
    while(1)
    {
        if (fread(&value, sizeof(value), 1, tape._file)!=1)
        {
            sort(Mass,Mass+RAM);
            snprintf(fileName, sizeof(fileName), "%d", j);
            VremTape[j]=fopen(fileName,"wb");
            for(i=0;i<RAM;i++)
            {
                fwrite(&Mass[i], sizeof(value), 1, VremTape[j]);
            }
            memset(Mass, 0, sizeof(Mass));
            fclose(VremTape[j]);
            break;
        }
        else
        {
            if(i<RAM)
            {
                Mass[i]=value;
                i++;
            }
            else
            {
                fseek( tape._file ,  -4 , SEEK_CUR );
                sort(Mass,Mass+RAM);
                snprintf(fileName, sizeof(fileName), "%d", j);
                VremTape[j]=fopen(fileName,"wb");
                for(i=0;i<RAM;i++)
                {
                    fwrite(&Mass[i], sizeof(value), 1, VremTape[j]);
                }
                memset(Mass, 0, sizeof(Mass));
                fclose(VremTape[j]);
                i=0;
                j++;
            }
        }
    }
    tape.RewindTape();
//слияние временных лент
    mergeTapes(outTape,RAM,CountOfVremTape);
    
}

/*
class TapeSorter
{
    using TapeCreator = std::function<ITape* ()>;

private:
    TapeCreator _tapeCreator;

public:
   TapeSorter(const TapeCreator& tapeCreator) : _tapeCreator(tapeCreator) { }

   ITape* Sort(IReadonlyTape* tape) const {

    }
};*/
 

int main(int argc,char *argv[])
{
    
    FileTape MainTape(argv[1],"rb+");
    Config conf("Config.txt");
    int Ram=conf.Ram_Size();
    if(Ram>=MainTape.SizeTape())
        Ram=MainTape.SizeTape();
    TapeSort(MainTape,Ram,argv[2]);
	return 0;
}
