//Importa a biblioteca wifi
#include <WiFi.h>

// Credenciais da rede wifi, para fazer o login
const char* ssid     = "ecg";
const char* password = "12345678";

// Define a porta, padrão é 80. (se mudar, por exemplo para 8080, será acessivel em : 192.168.0.1:8080 sendo que o ip variavel)
WiFiServer server(80);

// Variavel que guarda a requisição http
String header;

#define PLOTT_DATA 
#define MAX_BUFFER 1000

// definição das variaveis
uint32_t prevData[MAX_BUFFER];
uint32_t sumData=0;
uint32_t maxData=0;
uint32_t avgData=0;
uint32_t roundrobin=0;
uint32_t countData=0;
uint32_t period=0;
uint32_t lastperiod=0;
uint32_t millistimer=millis();
double frequency;
double beatspermin=0;
uint32_t newData;

// função pega os batimentos
void freqDetec() {
  if (countData==MAX_BUFFER) {
   if (prevData[roundrobin] < avgData*1.5 && newData >= avgData*1.5){ // cria um ponto médio
    period = millis()-millistimer;//Pega o periodo
    millistimer = millis();//reseta o tempo
    maxData = 0;
   }
  }
 roundrobin++;
 if (roundrobin >= MAX_BUFFER) {
    roundrobin=0;
 }
 if (countData<MAX_BUFFER) {
    countData++;
    sumData+=newData;
 } else {
    sumData+=newData-prevData[roundrobin];
 }
 avgData = sumData/countData;
 if (newData>maxData) {
  maxData = newData;
 }

 /* Consulte um cardiologista
 * Este é apenas um protótipo para estudos não o utilize para fins medicos
 * O mesmo não possui nenhuma garantia de funcionamento
 */
#ifdef PLOTT_DATA
  Serial.print(newData);
 Serial.print("\t");
 Serial.print(avgData);
 Serial.print("\t");
 Serial.print(avgData*1.5);
 Serial.print("\t");
 Serial.print(maxData);
 Serial.print("\t");
 Serial.println(beatspermin);
#endif
 prevData[roundrobin] = newData;//guarda os valores
}



void setup() {
  Serial.begin(115200);

  // Conectando no wifi com SSID e Password
  Serial.print("Conectando em ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Mostra o local de onde esta rodando o servidor
  Serial.println("");
  Serial.println("WiFi Conectado!");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  delay(5000);
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Lista os dispositivos conectados
            newData = analogRead(34);
            freqDetec();
            if (period!=lastperiod) {
               frequency = 1000/(double)period;//Calcula frequencia
               if (frequency*60 > 20 && frequency*60 < 140) { // Batimentos precisam estar entre 20 e 140 para ser considerado válido (dados não reais)
                beatspermin=frequency*60;
          #ifndef PLOTT_DATA
                  Serial.print(frequency);
                  Serial.print(" hz");
                  Serial.print(" ");
                  Serial.print(beatspermin);
                  Serial.println(" bpm");
                  delay(25);
          #endif
                  lastperiod=period;
               }
            }       

  if (client) {                             // Se tiver alguem conectado
    Serial.println("Novo cliente.");          // Mostra mensagem
    String currentLine = "";                
    while (client.connected()) {            // enquanto cliente tiver conectado
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Conexão: close");
            client.println();

            // Criando pagina html no esp32
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta http-equiv='refresh' content='1'>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
           
            // Pagina a ser exibida
            client.println(" <meta charset='UTF-8'><link rel='stylesheet' href='http://cheatsupreme.com/ufsc/bootstrap.css'/> <link rel='stylesheet' href='http://cheatsupreme.com/ufsc/main.css'/> <link rel='stylesheet' href='http://cheatsupreme.com/ufsc/theme.css'/> <link rel='stylesheet' href='http://cheatsupreme.com/ufsc/MoneAdmin.css'/> <link href='https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css' rel='stylesheet' integrity='sha384-wvfXpqpZZVQGK6TAh5PVlGOfQNHSoD2xbE+QkPxCAFlNEevoEH3Sl0sibVcOQVnN' crossorigin='anonymous'> <link href='http://cheatsupreme.com/ufsc/layout2.css' rel='stylesheet'/> <link href='http://cheatsupreme.com/ufsc/examples.css' rel='stylesheet'/> <link rel='stylesheet' href='http://cheatsupreme.com/ufsc/timeline.css'/> </head> <body class='padTop53 ' > <div id='left' > <div class='media user-media well-small'> <a class='user-link' href='#'> <img class='media-object img-thumbnail user-img' alt='User Picture' src='http://cheatsupreme.com/ufsc/1.jpg'/> </a> <br/> <div class='media-body'> <h5 class='media-heading'> Dr. Gregory House</h5> <ul class='list-unstyled user-info'> <li> <a class='btn btn-success btn-xs btn-circle' style='width: 10px;height: 12px;'></a> Online </li></ul> </div><br/> </div><ul id='menu' class='collapse'> <li class='panel active'> <a href='index.php' > <i class='fa fa-home' aria-hidden='true'></i> Geral </a> </li><li class='panel'> <a href='#pages_calendar.html' data-parent='#menu' data-toggle='collapse' class='accordion-toggle' data-target='#pagesr-nav'> <i class='fa fa-calendar-o'></i> Calendário </a> </li><li><a href='#tables.html'><i class='fa fa-database'></i> Dados de pacientes </a></li><li><a href='sair.php'><i class='fa fa-reply'></i> Sair da conta </a></li></ul> </div><div id='content'> <div class='inner' style='min-height: 700px;'> <div class='row'> <div class='col-lg-12'> <CENTER> <h1> CARDIOTIME - Cardiologia em tempo real </h1> </CENTER> </div></div><hr/> <div class='row'> <div class='col-lg-12'> <div style='text-align: center;'> <a class='quick-btn' href='#'> <i class='fa fa-check-square'></i> <span> Iniciar exame</span> <span class='label label-danger'>1</span> </a> <a class='quick-btn' href='#'> <i class='fa fa-envelope-open'></i> <span>Mensagens</span> <span class='label label-success'>2</span> </a> <a class='quick-btn' href='#'> <i class='fa fa-sort-alpha-desc'></i> <span>Antecedentes</span> <span class='label label-warning'>3</span> </a> <a class='quick-btn' href='#'> <i class='fa fa-heartbeat'></i> <span>Risco</span> <span class='label btn-metis-2'>4</span> </a> <a class='quick-btn' href='#'> <i class='fa fa-ambulance'></i> <span>Emergência</span> <span class='label btn-metis-4'>5</span> </a> <a class='quick-btn' href='#'> <i class='fa fa-microphone'></i> <span>Novo Audio</span> <span class='label label-default'>6</span> </a> </div></div></div><hr/> <div class='row'> <div class='col-lg-8'> <div class='panel panel-default'> <div class='panel-heading'> <center> <h4> ELETROCARDIOGRAMA</h4> </center> </div><div class='demo-container'> <div id='placeholderRT' class='demo-placeholder'></div></div></div></div><div class='col-lg-4'> <div class='chat-panel panel panel-primary'> <div class='panel-heading'> <i class='icon-comments'></i> Bate-papo <div class='btn-group pull-right'> <button type='button' data-toggle='dropdown'> <i class='icon-chevron-down'></i> </button> <ul class='dropdown-menu slidedown'> <li> <a href='#'> <i class='icon-refresh'></i> Atualizar </a> </li><li class='divider'></li><li> <a href='#'> <i class='icon-signout'></i> Ficar offline </a> </li></ul> </div></div><div class='panel-body'> <ul class='chat'> <li class='left clearfix'> <span class='chat-img pull-left'> <img src='http://cheatsupreme.com/ufsc/2.jpg' alt='User Avatar' class='img-circle'/> </span> <div class='chat-body clearfix'> <div class='header'> <strong class='primary-font'> Jack Sparrow </strong> <small class='pull-right text-muted'> <i class='icon-time'></i> 12 mins atrás </small> </div><br/> <p> Olá doutor, podemos começar o exame? </p></div></li><li class='right clearfix'> <span class='chat-img pull-right'> <img src='http://cheatsupreme.com/ufsc/1.jpg' alt='User Avatar' class='img-circle'/> </span> <div class='chat-body clearfix'> <div class='header'> <small class=' text-muted'> <i class='icon-time'></i> 13 mins atrás</small> <strong class='pull-right primary-font'> Dr. Gregory House</strong> </div><br/> <p> Claro, já esta com os aparelhos colocados nos locais indicados? </p></div></li><li class='left clearfix'> <span class='chat-img pull-left'> <img src='http://cheatsupreme.com/ufsc/2.jpg' alt='User Avatar' class='img-circle'/> </span> <div class='chat-body clearfix'> <div class='header'> <strong class='primary-font'> Jack Sparrow </strong> <small class='pull-right text-muted'> <i class='icon-time'></i> 12 mins atrás </small> </div><br/> <p> Sim, estão exatamente onde o senhor me orientou a colocar </p></div></li><li class='right clearfix'> <span class='chat-img pull-right'> <img src='http://cheatsupreme.com/ufsc/1.jpg' alt='User Avatar' class='img-circle'/> </span> <div class='chat-body clearfix'> <div class='header'> <small class=' text-muted'> <i class='icon-time'></i> 13 mins atrás</small> <strong class='pull-right primary-font'> Dr. Gregory House</strong> </div><br/> <p> Ótimo, vamos lá, respire fundo e tente não realizar movimentos bruscos. </p></div></li></ul> </div><div class='panel-footer'> <div class='input-group'> <input id='Text1' type='text' class='form-control input-sm' placeholder='Digite sua mensagem aqui...'/> <span class='input-group-btn'> <button class='btn btn-warning btn-sm' id='Button1'> Enviar </button> </span> </div></div></div></div></div><div class='row'> </div><div class='row'> </div></div></div><div id='right'> <div class='well well-small'> <ul class='list-unstyled'> <li> <center><img class='media-object img-thumbnail user-img' alt='User Picture' style=' width: 60px;' src='http://cheatsupreme.com/ufsc/gif.gif'/></center> </li><li>Paciente &nbsp; : <span>Juliano</span></li><li>Sexo: &nbsp; : <span> Masculino</span></li><li>Idade &nbsp; : <span>36 anos</span></li><li> <h3 style=' font-style: Marker Felt, fantasy;'> BPM: &nbsp;<span>");
            if (beatspermin == 0){
              client.println("<script language='javascript' type='text/javascript'>alert('Nenhum batimento detectado. Conecte o dispositivo!');</script>"); 
            }else{
               client.println(beatspermin);
            }
            client.println("</span></h3></li></ul> </div><div class='well well-small'> <button class='btn btn-block'> Ajuda </button> <button class='btn btn-primary btn-block'> Pausar teste</button> <button class='btn btn-info btn-block'> Retomar teste</button> <button class='btn btn-success btn-block'> Novo exame </button> <button class='btn btn-danger btn-block'> Finalizar exame </button> <button class='btn btn-warning btn-block'> Extrato de contas </button> <button class='btn btn-inverse btn-block'> Finalização do exame </button> </div><div class='well well-small'> <span>Ansiedade</span><span class='pull-right'><small>55%</small></span> <div class='progress mini'> <div class='progress-bar progress-bar-info' style='width: 55%'></div></div><span>Arritmia Cardiaca</span><span class='pull-right'><small>60%</small></span> <div class='progress mini'> <div class='progress-bar progress-bar-success' style='width: 60%'></div></div><span>Variação</span><span class='pull-right'><small>38%</small></span> <div class='progress mini'> <div class='progress-bar progress-bar-warning' style='width: 38%'></div></div><span>Geral</span><span class='pull-right'><small>76%</small></span> <div class='progress mini'> <div class='progress-bar progress-bar-danger' style='width: 76%'></div></div></div></div></div><div id='footer'> <p>&copy; UFSC &nbsp;2018 - 2 &nbsp;</p></div><script src='http://cheatsupreme.com/ufsc/jquery-2.0.3.min.js'></script> <script src='http://cheatsupreme.com/ufsc/bootstrap.min.js'></script> <script src='http://cheatsupreme.com/ufsc/modernizr-2.6.2-respond-1.1.0.min.js'></script> <script src='http://cheatsupreme.com/ufsc/jquery.flot.js'></script> <script src='http://cheatsupreme.com/ufsc/jquery.flot.resize.js'></script> <script src='http://cheatsupreme.com/ufsc/jquery.flot.time.js'></script> <script src='http://cheatsupreme.com/ufsc/jquery.flot.stack.js'></script><script src='http://cheatsupreme.com/ufsc/for_index.js'></script> </body> </html>");
            
            // termina com linha branca
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    // Limpa variavel
    header = "";
    // fecha a conexão
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }
}
