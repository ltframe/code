
let reader= new FileReader();
let json;
let instance;

$(document).ready(function(){
  document.getElementById('file').onclick = function(e) {
    e.target.value = ''
  }
  setTimeout(()=>{
      console.log("app.js loaded");
      instance = new Module.myclass();
      $(".startloading").hide();
  }, 2000);
})
function save()
{
  const obj = Object.create({});
  var tables = $(".sheetlist").children();
  var tablearray = [];
  for (var i = 0; i < tables.length; i++) {
      tablearray.push($(tables[i]).text())
  }
  obj.data = {"first":{}};
  obj.info = {"count":tables.length,"tablenames":tablearray}
  let trlist = $("table tbody tr").not("#letter");
  let ls = [];
  let trarr = [];
  for (var i =0; i < trlist.length; i++) {        
      let tdlist = $(trlist[i]).children();
      let tdarr = [];
      for (var j =1; j < tdlist.length; j++) {
          let td = $(tdlist[j]);
          var t = td.text();
          tdarr.push(t);
      }
      trarr.push(tdarr);
  }
  ls.push(trarr);
  obj.data.first = trarr;
  instance.saveexcel(JSON.stringify(obj));
  savetodisk();
}
function tablist()
{
    let str = '';      
    for (var i = 0; i < json.info.tablenames.length; i++) {
        if(i==0){
            str +=`<div _na="${json.info.tablenames[i]}" class="currentsheet">${json.info.tablenames[i]}</div>`;
        }else{
                str +=`<div _na="${json.info.tablenames[i]}">${json.info.tablenames[i]}</div>`;
        }
    }    
    $(".sheetlist").append(str);

    $(".sheetlist>div").each(function()
    {
        $(this).click(function(){
             $(".sheetlist>div").each(function(){
                    $(this).removeClass('currentsheet')
             })
             $(this).addClass('currentsheet');
             celllist($(this).text())
        })
    })
}
function celllist(key)
{
  let str = '';
  for (var j = 0; j < json.data[key].length; j++) {
      str +=  `<tr style="height: 38px;" >
                          <td class="drug-ele-td" style="width: 48px; text-align: center;">${j+1}</td>`;
      for (var i = 0; i < 10; i++) {
          if(json.data[key][j][i]){
              str +=`<td style="width: 90px;">${json.data[key][j][i]}</td>`;
          }else{
              str +=`<td style="width: 90px;"></td>`;
          }
      }                    
      str +=`</tr>`;
  } 
  $("table tbody tr").not("#letter").remove()
  $("table tbody").append(str);
  $("td").not('.drug-ele-td').each(function()
  {
      $(this).click(function(){
          $(this).attr('contenteditable',"true");
          $(this).addClass('contenteditable',"true")
      })
  })
}
function myfun(e) {
    
    let files = document.getElementById('file').files;
    file=files[0];
    reader.addEventListener('loadend', writefile);
    reader.readAsArrayBuffer(file);
    setTimeout(()=>{
        
    	  let jsonstr = instance.loadexcel(file.name);
        $(".sheetlist").empty();
        json = JSON.parse(jsonstr);
        tablist();
        celllist(Object.keys(json.data)[0]);          
        console.log('result: ' + jsonstr);            
    },1000)
}


function writefile(e){
    let result=reader.result;
    const uint8_view = new Uint8Array(result);
    FS.writeFile('/data/'+file.name, uint8_view)
}



function savetodisk() 
{
   var data=FS.readFile("/data/demo.xls");
   var blob;
   blob = new Blob([data.buffer], {type: "application/vnd.ms-excel"});
   saveAs(blob, "demo.xls");
}