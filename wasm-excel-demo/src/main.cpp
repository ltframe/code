#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <locale.h>
#include "BasicExcel.hpp"
#include <limits.h>
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <sys/io.h>
#include <sys/sysinfo.h>
#include "cJSON.h"
using namespace YExcel;
using namespace std;
using namespace emscripten;



wstring StringToWString(const std::string &str)
{
      std::wstring wstr(str.length(),L' ');
      std::copy(str.begin(), str.end(), wstr.begin());
      return wstr; 
}
class myclass
{
  public:
    wstring outputstr = L"";
    myclass()
    {

    }
    void saveexcel(string value)
    {
        cJSON  *json = cJSON_Parse(value.c_str());
        cJSON *info = cJSON_GetObjectItem(json, "info");
        cJSON *count = cJSON_GetObjectItem(info, "count");
        cJSON *tablenames = cJSON_GetObjectItem(info, "tablenames");
        int _count = count->valueint;
        cJSON  *data = cJSON_GetObjectItem(json, "data");


        BasicExcel *e = new BasicExcel();
        e->New(_count);
        


        for (int i = 0; i < 1; i++)
        {
          BasicExcelWorksheet* sheet = e->GetWorksheet((unsigned)i);
          cJSON *name = cJSON_GetArrayItem(tablenames, i);
          sheet->Rename(StringToWString(name->valuestring).c_str());

          cJSON *list = cJSON_GetObjectItem(data, name->valuestring);

          for (int j = 0; j < cJSON_GetArraySize(list); j++)
          {
            cJSON *item = cJSON_GetArrayItem(list,j);

            for (size_t k = 0; k < cJSON_GetArraySize(item); k++)
            {
              cJSON *c = cJSON_GetArrayItem(item, k);

              BasicExcelCell* cell = sheet->Cell(j, k);
              cell->SetWString(StringToWString(c->valuestring).c_str());
            }     
          }
        }
        e->SaveAs("/data/demo.xls");
    }
    void readsheet(BasicExcelWorksheet* sheet1)
    {
      //获取行数
      size_t rows = sheet1->GetTotalRows();
      //获取列数
      size_t cols = sheet1->GetTotalCols();
      for (size_t r = 0; r < rows; r++) {
        outputstr+=L"[";
        //wprintf(L"\n");
        for (size_t c = 0; c < cols; c++)
        {
          //获取r行，c列的内容
          BasicExcelCell* cell = sheet1->Cell(r, c);
          //判断单元格类型
          switch (cell->Type())
          {
          case BasicExcelCell::WSTRING:
          {
            const wchar_t* ret = cell->GetWString();
            outputstr += L"\"";
            //outputstr += keys[c];
            //outputstr += L"\"";
            //outputstr += L":\"";
            outputstr += ret;
            outputstr += L"\"";
            if (c != cols - 1) {
              outputstr += L",";
            }
           
          }
          break;    
          default:
            break;
          }
        }

        outputstr+=L"]";
        if(r!=rows-1){
          outputstr+=L",";
        }
      }
    }
    wstring loadexcel(string fb)
    {
       outputstr += L"{\"info\":{";
       BasicExcel *e  =new BasicExcel;
       wprintf(L"%ls\r\n",L"start load...");
       string fname = string("/data/")+fb;
       if (!e->Load(fname.c_str()))
       {
            wprintf(L"%ls\r\n",L"load failed");
            return wstring(L"load failed\n");
       }
       int sheetscount = e->GetTotalWorkSheets();
       outputstr +=L"\"count\":"+ std::to_wstring(sheetscount);
       outputstr+=L",\"tablenames\":[";
       for (size_t i = 0; i < sheetscount; i++)
       {
          outputstr+=L"\"";
          outputstr+=e->GetUnicodeSheetName(i);
          outputstr+=L"\"";
          if(i!=sheetscount-1){
              outputstr+=L",";
          }
       }
       outputstr+=L"]},\"data\":{";
        for (size_t i = 0; i < sheetscount; i++) {
            BasicExcelWorksheet* sheet1 = e->GetWorksheet(i);
            if (sheet1) {
              outputstr += L"\"";
              outputstr += e->GetUnicodeSheetName(i);
              outputstr += L"\":[";
              readsheet(sheet1);

              outputstr += L"]";
              if(i!=sheetscount-1){
                outputstr += L",";
              }
            }
        }
       outputstr += L"}}";
       wprintf(L"%ls",outputstr.c_str());
       return outputstr;
    }
};






EMSCRIPTEN_BINDINGS(myclass) {
  class_<myclass>("myclass")
  	.constructor()
    .function("loadexcel", &myclass::loadexcel)
    .function("saveexcel", &myclass::saveexcel)
    ;
}
 void setup_IDBFS(){
      EM_ASM(
          FS.mkdir('/data');
        FS.mount(IDBFS,{},'/data');
      );
    }
int main()
{
  setup_IDBFS();
  setlocale(LC_ALL, "chs");
  return 0;
}
