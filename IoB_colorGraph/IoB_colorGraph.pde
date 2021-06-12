/***************************************************
 Internet of Bananas
 A critical design project to collect data from bananas
 
 IoB is developed by Artur Cordeiro + Mattia Thibault
 https://github.com/Internet-of-Bananas
 Fortaleza / Tampere, 2021.
 
 This sketch creates a visualization of the bananas' color around 12:00GMT
 
 */


color cores [][] = {
  {#adb25e, #b7b261, #b8af5e, #b7aa5d, #a28c37, #5c3600, #060000, #000000}, 
  {#beb277, #b3a469, #a09256, #8b7d3e, #786a2c, #584d12, #000000, #000000}, 
  {#9c7d13, #966e00, #865600, #744000, #6a3500, #3d0000, #2a0000, #000000}, 
  {#a3974a, #9a8834, #a28631, #a8802c, #ad833d, #986822, #86510d, #622400}, 
  {#c1b87b, #c5b981, #c6b883, #c4b582, #c0b080, #b2a070, #9e895c, #7c673f}, 
  {#9f8f12, #ab921c, #b19226, #916800, #835600, #713f00, #602800, #501500}, 
  {#f3d0a8, #debf93, #cbae7f, #d4ad79, #dab585, #d1ac7d, #a06c42, #83623b}, 
  {#b1913a, #b89343, #b89148, #b88f49, #b18641, #ac7f3c, #a47434, #a06e2f}
};

String datas[] = {"jun 5", "jun 6", "jun 7", "jun 8", "jun 9", "jun 10", "jun 11", "jun 12"};

String nomes [] = {
  "Ba Nana", 
  "Liliana", 
  "Bananenstein", 
  "Prata", 
  "Banano and Banana", 
  "Bananatrooper", 
  "Benjamin Banana", 
  "Baaaaanaaaanaaa"
};

int txtAltura = 25;
int raio;
int yOffSet = 40;

void setup() {
  size(1024, 768);
}


void draw() {
  background(#8a8a8a);
  fill(#fee348);
  textSize(txtAltura);
  for (int i = 0; i < 8; i++) {
    text(nomes[i], 20, txtAltura + 25 + (height/9 * i) + yOffSet);
  }
  int x0 = int(textWidth(20+nomes[4]));
  raio = (width-x0 - yOffSet)/8 ;

  stroke(50);

  for (int i = 0; i < nomes.length; i++) {
    for (int j = 0; j < datas.length; j++) {
      fill(cores[i][j]);
      rect(x0 + (width-x0 - yOffSet)/8 * j, yOffSet+ (height/9 * i), raio, raio);
    }
  }

  textSize(txtAltura-7);

  for (int j = 0; j < datas.length; j++) {
    fill(#fee348);
    text(datas[j], x0 + 10 + (width-x0- yOffSet)/8 * j, (height/9 * 9) - 15);
  }

  //saveFrame("IoB_colorGraph.jpg"); // Uncomment to save the image file.
  noLoop();
}
