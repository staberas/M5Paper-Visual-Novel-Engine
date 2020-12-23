#include <M5EPD.h>
#include <ArduinoJson.h>

    int randNumber;
    //page current
    int pages = 0;
    char* json;
    const char *filename = "/noveldemo.txt"; //here is the novel json file
    String pageobjx = "";
    M5EPD_Canvas canvas(&M5.EPD);
    int point[2][2];
    //game logic
    int hp = 100;
    int dmg = 120;
    String batlog;
    String pagename;
    boolean batticon = true;
      
void setup()
{
    randomSeed(analogRead(0));//no need for random
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    //overlay bg on full screen
    canvas.createCanvas(540, 960);
    canvas.setTextSize(5);
    canvas.drawString("FAILED TO LOAD FILE", 5, 5);
    //loadnovel();
    //jpg must be in 90degree (full screen)
    canvas.drawJpgFile(SD, "/u11.jpg");
    avatarload();
    //load the scene starting at x0 y0 position
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
    //after loading we kill canvas and rotate to overlay the text on the bg 
    canvas.deleteCanvas();
    M5.EPD.SetRotation(0);
    //set the new size of the scene-element (canvas)
    canvas.createCanvas(650, 140);
    canvas.setTextSize(3);
    loadnovel(pages,"text");
    String prevsrc = pageobjx; 
    canvas.drawString(pageobjx, 5, 5);
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    //kill canvas overlay main image 
    canvas.deleteCanvas();
    M5.EPD.SetRotation(90);
    //create a draw area that will overwrite the screen 
    canvas.createCanvas(361,642);
    //pull image from sd card
    loadnovel(pages,"mainscreenshot");
    canvas.drawPngFile(SD, pageobjx.c_str());
    //create it on position with selected update mode  
    canvas.pushCanvas(150,280,UPDATE_MODE_GC16);
    //load items 
    //nextload(true); 
    //prevload(true);
    avatarload();
    
}

void loop()
{
    
  if (M5.BtnP.read() && M5.BtnP.pressedFor(500) && batticon){
    canvas.deleteCanvas();
    M5.EPD.SetRotation(0);
    canvas.createCanvas(80, 40);
    canvas.setTextSize(4);
    // Battery
    uint32_t vol = M5.getBatteryVoltage();
    if(vol < 3300)
    {
        vol = 3300;
    }
    else if(vol > 4350)
    {
        vol = 4350;
    }
    float battery = (float)(vol - 3300) / (float)(4350 - 3300);
    if(battery <= 0.01)
    {
        battery = 0.01;
    }
    if(battery > 1)
    {
        battery = 1;
    }
    uint8_t px = battery * 25;
    canvas.setTextArea(5,5,80,40);
    int battpercent = battery*100;
    canvas.println( String(battpercent)+"%");
    //printf( "%d%%", (int)(battery * 100));
    canvas.pushCanvas(0,0,UPDATE_MODE_GL16);
    batticon = false;
  };

  if (M5.BtnR.read()){
    //next page button
    Serial.println(M5.BtnR.read());
    M5.EPD.SetRotation(0);
    canvas.deleteCanvas();
    canvas.createCanvas(650, 140);
    canvas.setTextSize(3);
    loadnovel(pages,"avatar");
    String prevsrc = pageobjx; 
    pages++;
    loadnovel(pages, "avatar");
    if(prevsrc != pageobjx){
    batlog = "Loading next page";
    canvas.println(batlog);  
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
      actiondone(true);
      batticon = true;
    }else{
    canvas.setTextArea(200,400,500,140);
    loadnovel(pages,"text");
    canvas.println(pageobjx.c_str());
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    delay(2000);
    }
    loadinteractions(selectionblock);
  };
  
  if(M5.BtnL.read()){
    //prev page button
    Serial.println(M5.BtnL.read());
    M5.EPD.SetRotation(0);
    canvas.deleteCanvas();
    canvas.createCanvas(650, 140);
    canvas.setTextSize(3);
    loadnovel(pages,"avatar");
    String prevsrc = pageobjx;  
    pages--;
    loadnovel(pages,"avatar");
    if(prevsrc != pageobjx){
    batlog = "Loading previous page";
    canvas.println(batlog);  
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    actiondone(true);
    batticon = true;
    }else{
    canvas.setTextArea(200,400,500,140);
    loadnovel(pages,"text");
    canvas.println(pageobjx.c_str());
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    delay(2000); 
    }
  };
  bool valueupdate = false;
  //TOUCH INTERFACE
    if(M5.TP.avaliable()){
        if(!M5.TP.isFingerUp()){
            M5.TP.update();
            //canvas.fillCanvas(0);
            bool is_update = false;
            for(int i=0;i<2; i++){
                tp_finger_t FingerItem = M5.TP.readFinger(i);
                if((point[i][0]!=FingerItem.x)||(point[i][1]!=FingerItem.y)){
                    is_update = true;
                    valueupdate = true;
                    point[i][0] = FingerItem.x;
                    point[i][1] = FingerItem.y;
                    //canvas.fillRect(FingerItem.x-50, FingerItem.y-50, 100, 100, 15);
                    Serial.printf("Finger ID:%d-->X: %d*C  Y: %d  Size: %d\r\n", FingerItem.id, FingerItem.x, FingerItem.y , FingerItem.size);
                }
            }
            if(is_update)
            {
               // canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
              //trigger when any touch action happens
            }
        }
    }
    //TOUCH interface end
    
    //Touch UI for routes
    if(selectionnum > 0){
      if ((M5.TP.readFingerX(0) > 320) && (M5.TP.readFingerY(0) > 30) && (M5.TP.readFingerX(0) < 600) && (M5.TP.readFingerY(0) < 80) && (selectionnum >= 1) ){
      Serial.println("choice 1 route");
      decisiontree(1);
      }
      if ((M5.TP.readFingerX(0) > 320) && (M5.TP.readFingerY(0) > 120) && (M5.TP.readFingerX(0) < 600) && (M5.TP.readFingerY(0) < 170) && (selectionnum >= 2) ){
      Serial.println("choice 2 route");
      //  canvas.fillCircle(320,20,20,5);
      decisiontree(2);     
      }
      if ((M5.TP.readFingerX(0) > 320) && (M5.TP.readFingerY(0) > 190) && (M5.TP.readFingerX(0) < 600) && (M5.TP.readFingerY(0) < 240) && (selectionnum >= 3) ){
      Serial.println("choice 3 route"); 
      //  canvas.fillCircle(320,20,20,5);
      decisiontree(3);      
      }
      if ((M5.TP.readFingerX(0) > 320) && (M5.TP.readFingerY(0) > 260) && (M5.TP.readFingerX(0) < 600) && (M5.TP.readFingerY(0) < 310) && (selectionnum == 4) ){
      Serial.println("choice 4 route");
      //  canvas.fillCircle(320,20,20,5);
      decisiontree(4);
      }                  
    }
//main game logic or anything else



//next butt (check serial for finger to screen position before assinging)
if ((M5.TP.readFingerX(0) > 10) && (M5.TP.readFingerY(0) > 65) && (M5.TP.readFingerX(0) < 120) && (M5.TP.readFingerY(0) < 160) && valueupdate)
{
    M5.EPD.SetRotation(0);
    canvas.deleteCanvas();
    canvas.createCanvas(650, 140);
    canvas.setTextSize(3);
    loadnovel(pages,"avatar");
    String prevsrc = pageobjx; 
    pages++;
    loadnovel(pages, "avatar");
    //pagename = "page"+String(pages);
    //pageobjx = root[pagename][0];
    if(prevsrc != pageobjx){
    batlog = "Loading next page";
    canvas.println(batlog);  
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
      actiondone(true);
    }else{
    canvas.setTextArea(200,400,500,140);
    loadnovel(pages,"text");
    canvas.println(pageobjx.c_str());
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    delay(2000);
    }
    loadinteractions(selectionblock);
}
//prev butt
if ((M5.TP.readFingerX(0) > 100) && (M5.TP.readFingerY(0) > 65) && (M5.TP.readFingerX(0) < 170) && (M5.TP.readFingerY(0) < 160) && valueupdate)
{
    M5.EPD.SetRotation(0);
    canvas.deleteCanvas();
    canvas.createCanvas(650, 140);
    canvas.setTextSize(3);
    loadnovel(pages,"avatar");
    String prevsrc = pageobjx;  
    pages--;
    loadnovel(pages,"avatar");
    //pagename = "page"+String(pages);
    //pageobjx = root[pagename][0];
    if(prevsrc != pageobjx){
    batlog = "Loading previous page";
    canvas.println(batlog);  
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    actiondone(true);
    }else{
    canvas.setTextArea(200,400,500,140);
    loadnovel(pages,"text");
    canvas.println(pageobjx.c_str());
    //canvas.println(pageobjx["text"].as<String>());
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    delay(2000); 
    }
}

//game ui load 
    
}
//no longer needed? mayber for different UI actions 
void nextload(boolean checker){
 //load items
 if (checker){
  M5.EPD.SetRotation(0);              
  canvas.deleteCanvas();
  canvas.createCanvas(80,80);
  canvas.drawPngFile(SD, "/item8BIT_next.png",0,0,80,80,0,0, 5.0);
  canvas.pushCanvas(20,100,UPDATE_MODE_GL16);
 }
}

//no longer needed? mayber for different UI actions 
void prevload(boolean checker){
 //load items
 if (checker){
  M5.EPD.SetRotation(0);              
  canvas.deleteCanvas();
  canvas.createCanvas(100,100);
  canvas.drawPngFile(SD, "/item8BIT_prev.png",0,0,100,100,0,0, 5.0, 1);
  canvas.pushCanvas(20,250,UPDATE_MODE_GC16);
 }
}

void actiondone(boolean checker){
    Serial.printf("action");
    //hp -= random(10);
    //dmg += random(10);
    //refix the screen if new images exit
    canvas.deleteCanvas();
    M5.EPD.SetRotation(90);
    //M5.EPD.Clear(true);
    //overlay bg on full screen
    canvas.createCanvas(540, 960);
    canvas.setTextSize(5);
    //jpg must be in 90degree (full screen)
    canvas.drawJpgFile(SD, "/u11.jpg");

    avatarload();

    //load the scene starting at x0 y0 position
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
    
    //kill canvas overlay main image 
    canvas.deleteCanvas();
    M5.EPD.SetRotation(90);
    //create a draw area that will overwrite the screen 
    canvas.createCanvas(361,642);
    //pull image from sd card
    loadnovel(pages,"mainscreenshot");
    canvas.drawPngFile(SD, pageobjx.c_str());
    //canvas.drawPngFile(SD, pageobjx["mainscreenshot"].asString());
    //create it on position with selected update mode  
    canvas.pushCanvas(150,280,UPDATE_MODE_GC16);

    //end of refix
    M5.EPD.SetRotation(0);
    canvas.deleteCanvas();//710
    canvas.createCanvas(650, 140);
    canvas.setTextSize(3);
    //canvas.setTextWrap(true,true);
    canvas.setTextArea(200,400,500,140);
    loadnovel(pages,"text");
    canvas.println(pageobjx);
    //newline wrap
    canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
    delay(2000); // waits for a second

}

void loadnovel(int loadpage, String item){
  //loadpage is pages
 Serial.println("  sd load  ");
 String pagenamed = "page"+String(loadpage);
 //stop pages to move into a negative value
 if(loadpage < 0){ 
  pages = 0;
  pagenamed = "page0";
  }
//json mem pool
const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 240;
DynamicJsonBuffer jsonBuffer(capacity);
File file = SD.open(filename);
//const char* json = "{\"page0\":[{\"avatar\":\"/GoddessPriest1.png\",\"text\":\"Press the BOOK to begin..\",\"mainscreenshot\":\"/bath_rooms.png\"}],\"page1\":[{\"avatar\":\"/GoddessPriest11.png\",\"text\":\"You may rest in the embrace of Numen Terra ...\",\"mainscreenshot\":\"/bath_rooms.png\"}],\"page2\":[{\"avatar\":\"/GoddessPriest11.png\",\"text\":\"He said with a bitter smile on his face , but i was sure like everyone in the room, that he would burn in hell. \",\"mainscreenshot\":\"/bath_rooms.png\"}]}";
JsonObject& root = jsonBuffer.parseObject(file);
JsonVariant pageobj = root[pagenamed][0];
pageobjx = pageobj[item].as<String>();
//interactivity on page
Serial.println(pageobj["choices"][0].as<String>());
Serial.println(pageobj["choices"][0]["choice1"].as<String>());
if( pageobj["jumps"][0].success() || pageobj["choices"][0].success() ){  //fix line
  selectionblock = true;
  Serial.println("selectionblock true");
}
if(root.size() <= pages){
Serial.println("no-new-page in json ");
pages--;
pagenamed = "page"+String(loadpage--);
canvas.println("This was the last page... Reloading");
//newline wrap
canvas.pushCanvas(280,400,UPDATE_MODE_GL16);
delay(1000);
}
file.close();
//endjson
}

void avatarload(){
  canvas.drawPngFile(SD, "/item8BIT_prev.png",400,100,80,80,0,0, 5.0);
  canvas.drawPngFile(SD, "/item8BIT_next.png",400,10,80,80,0,0, 5.0);
  //canvas.drawPngFile(SD, pageobjx["avatar"].asString(),0,0);
  loadnovel(pages,"avatar");
  canvas.drawPngFile(SD, pageobjx.c_str(),0,0);
  //canvas.drawPngFile(SD, pageobjx,0,0);
}

void loadinteractions(boolean selectionblock){
//if true load the ui 
Serial.println("inside in loadinteraction");
if (selectionblock){
  //canvas
  Serial.println("interaction true");
  //canvas.setTextArea(200,150,150,150);
  //end canvas
const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 240;
DynamicJsonBuffer jsonBuffer(capacity);
File file = SD.open(filename);
JsonObject& root = jsonBuffer.parseObject(file);
JsonVariant pageobj = root["page"+String(pages)][0]["choices"][0];
//interactivity on page
int i = 1;
String choicenum = "choice"+String(i);
pageobjx = pageobj[choicenum].as<String>();
Serial.println(pageobjx);
while(pageobjx.length() > 0){
//pageobj.["choice"+String(i)]
//---canvas.println(pageobjx);
if(i == 1){ 
  canvas.deleteCanvas();//
  canvas.createCanvas(250, 40);
  canvas.setTextSize(3);
  canvas.fillCircle(0,20,20,5);
  canvas.fillCircle(250,20,20,5);
  if(pageobjx.length() > 3){ 
    canvas.drawString(pageobjx, pageobjx.length()-3*-15, 10); 
    }
  else{ 
    canvas.drawString(pageobjx, 100, 10); 
    }
  canvas.pushCanvas(320,50,UPDATE_MODE_GL16);
 }else if (i ==  2){ 
  canvas.deleteCanvas();//
  canvas.createCanvas(250, 40);
  canvas.setTextSize(3);
  canvas.fillCircle(0,20,20,5);  
  canvas.fillCircle(250,20,20,5);
  if(pageobjx.length() > 3){ 
    canvas.drawString(pageobjx, pageobjx.length()-3*-15, 10); 
    }
  else{ 
    canvas.drawString(pageobjx, 100, 10); 
    } 
  canvas.pushCanvas(320,120,UPDATE_MODE_GL16);
 } else if(i == 3){ 
  canvas.deleteCanvas();//
  canvas.createCanvas(250, 40);
  canvas.setTextSize(3);
  canvas.fillCircle(0,20,20,5);  
  canvas.fillCircle(250,20,20,5);  
  if(pageobjx.length() > 3){ 
    canvas.drawString(pageobjx, pageobjx.length()-3*-15, 10); 
    }
  else{ 
    canvas.drawString(pageobjx, 100, 10); 
    }  
  canvas.pushCanvas(320,190,UPDATE_MODE_GL16);
 }else{ 
  canvas.deleteCanvas();//
  canvas.createCanvas(250, 40);
  canvas.setTextSize(3);
  canvas.fillCircle(0,20,20,5);  
  canvas.fillCircle(250,20,20,5);  
  if(pageobjx.length() > 3){ 
    canvas.drawString(pageobjx, pageobjx.length()-3*-15, 10); 
    }
  else{ 
    canvas.drawString(pageobjx, 100, 10); 
    }
  canvas.pushCanvas(320,260,UPDATE_MODE_GL16);
 }
//canvas.println("This");
i++;
choicenum = "choice"+String(i);
pageobjx = pageobj[choicenum].as<String>();
selectionnum = i-1;
}
//newline wrap
//canvas.pushCanvas(280,200,UPDATE_MODE_GL16);
delay(1000);
  }
selectionblock = false;
}

void decisiontree(int selected){

  //check json and do action
const size_t capacity = 2*JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 240;
DynamicJsonBuffer jsonBuffer(capacity);
File file = SD.open(filename);
JsonObject& root = jsonBuffer.parseObject(file);
JsonVariant pageobj = root["page"+String(pages)][0]["jumps"][0];
//jump to page or chapter (file) 
String jumpnum = "jump"+String(selected);
pageobjx = pageobj[jumpnum].as<String>();
Serial.println(pageobjx);
if(String(pageobjx.charAt(0)) == "p"){
  //page skip interaction
  Serial.println("Page SKIP");
  pageobjx.replace("page"," ");
  pages = pageobjx.toInt();
  selectionnum = 0;
  actiondone(true);
}
if(String(pageobjx.charAt(0)) == "/"){
  //change chapter (filename at root)
  Serial.println("setting new Filename for chapter");
  filename = pageobjx;
  Serial.println(pageobjx);
  pages = 0;
  selectionnum = 0;    
  actiondone(true);
 }
}
