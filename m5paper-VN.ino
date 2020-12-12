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