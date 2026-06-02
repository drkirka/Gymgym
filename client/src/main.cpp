#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using Sock=SOCKET;
const Sock badsock=INVALID_SOCKET;
void cls(Sock s){closesocket(s);}
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
using Sock=int;
const Sock badsock=-1;
void cls(Sock s){close(s);}
#endif
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;
struct User{std::string n,g,l,x;int d{},m{};};
std::string env(const char* k,const char* v){auto p=std::getenv(k);return p?p:v;}
std::string esc(std::string s){for(auto& c:s)if(c==' ')c='_';return s;}
class Net{
std::string h;int p;Sock fd=badsock;
public:
Net(std::string a,int b):h(std::move(a)),p(b){
#ifdef _WIN32
WSADATA w;WSAStartup(MAKEWORD(2,2),&w);
#endif
}
~Net(){
if(fd!=badsock)cls(fd);
#ifdef _WIN32
WSACleanup();
#endif
}
Net(const Net&)=delete;
Net& operator=(const Net&)=delete;
Net(Net&& o)noexcept:h(std::move(o.h)),p(o.p),fd(o.fd){o.fd=badsock;}
Net& operator=(Net&& o)noexcept{if(this!=&o){if(fd!=badsock)cls(fd);h=std::move(o.h);p=o.p;fd=o.fd;o.fd=badsock;}return *this;}
bool con(){
if(fd!=badsock)return true;
addrinfo hi{},*r=nullptr;
hi.ai_family=AF_UNSPEC;
hi.ai_socktype=SOCK_STREAM;
auto ps=std::to_string(p);
if(getaddrinfo(h.c_str(),ps.c_str(),&hi,&r)!=0)return false;
for(auto q=r;q;q=q->ai_next){
Sock s=socket(q->ai_family,q->ai_socktype,q->ai_protocol);
if(s==badsock)continue;
if(connect(s,q->ai_addr,(int)q->ai_addrlen)==0){fd=s;break;}
cls(s);
}
freeaddrinfo(r);
return fd!=badsock;
}
std::string cmd(const std::string& s){
if(!con())return "ERROR Client cannot connect to server\n";
std::string a=s+"\n";
const char* b=a.c_str();
int l=(int)a.size();
while(l){
int n=send(fd,b,l,0);
if(n<=0){cls(fd);fd=badsock;return "ERROR Send failed\n";}
b+=n;
l-=n;
}
char buf[4096];
std::memset(buf,0,sizeof(buf));
int n=recv(fd,buf,sizeof(buf)-1,0);
if(n<=0){cls(fd);fd=badsock;return "ERROR Server disconnected\n";}
return std::string(buf,n);
}
};
class App{
Net net;
std::vector<User> c;
std::string msg;
std::vector<std::string> g{"Muscle Gain","Weight Loss","General Fitness"};
std::vector<std::string> gk{"muscle_gain","weight_loss","general_fitness"};
std::vector<std::string> l{"Beginner","Intermediate","Advanced / Athlete"};
std::vector<std::string> lk{"beginner","intermediate","advanced"};
std::vector<std::string> ds{"1 day","2 days","3 days","4 days","5 days","6 days","7 days"};
std::vector<std::string> du{"30 min","45 min","60 min","75 min","90 min","120 min"};
std::vector<int> dv{30,45,60,75,90,120};
public:
App():net(env("GYMGYM_HOST","127.0.0.1"),std::stoi(env("GYMGYM_PORT","8080"))){ }
void run(){
auto sc=ScreenInteractive::TerminalOutput();
int sel=0;
std::vector<std::string> e{"Create user","View user","Get plan","Server status","Ping server","Branches","Help","Login","Profile","Logout","Exit"};
auto mn=Menu(&e,&sel);
auto bt=Button("Enter",[&]{sc.ExitLoopClosure()();});
auto box=Container::Vertical({mn,bt});
auto rd=Renderer(box,[&]{return vbox({text("Smart Gym Network")|bold|color(Color::Cyan)|hcenter,text("FTXUI client")|color(Color::GrayLight)|hcenter,separator(),mn->Render()|border,bt->Render()|hcenter,msg.empty()?text(""):text(msg)|color(Color::Green)|hcenter})|border;});
while(true){
sc.Loop(rd);
if(sel==0)add();
else if(sel==1)view();
else if(sel==2)plan();
else if(sel==3)stat();
else if(sel==4)msg=net.cmd("PING");
else if(sel==5)msg=net.cmd("BRANCHES");
else if(sel==6)msg=net.cmd("HELP");
else if(sel==7)login();
else if(sel==8)msg=net.cmd("PROFILE");
else if(sel==9)msg=net.cmd("LOGOUT");
else break;
}
}
private:
std::optional<User> find(const std::string& n){
auto it=std::find_if(c.begin(),c.end(),[&](const User& u){return u.n==n;});
if(it==c.end())return {};
return *it;
}
void add(){
auto sc=ScreenInteractive::TerminalOutput();
std::string n,x,ms;
int gi=0,li=0,di=2,ui=2;
auto ni=Input(&n,"name");
auto xi=Input(&x,"none");
auto gm=Radiobox(&g,&gi);
auto lm=Radiobox(&l,&li);
auto dm=Radiobox(&ds,&di);
auto um=Radiobox(&du,&ui);
auto sv=Button("Save",[&]{
if(n.empty()||x.empty()){ms="Fill all fields";return;}
User u{n,gk[gi],lk[li],x,di+1,dv[ui]};
std::string q="CREATE_USER "+esc(u.n)+" "+u.g+" "+u.l+" "+std::to_string(u.d)+" "+std::to_string(u.m)+" "+esc(u.x);
ms=net.cmd(q);
if(ms.rfind("OK",0)==0){
auto it=std::find_if(c.begin(),c.end(),[&](const User& a){return a.n==u.n;});
if(it==c.end())c.push_back(u);else *it=u;
msg=ms;
sc.ExitLoopClosure()();
}
});
auto bk=Button("Back",sc.ExitLoopClosure());
auto ct=Container::Vertical({ni,gm,lm,dm,um,xi,Container::Horizontal({sv,bk})});
auto rd=Renderer(ct,[&]{return vbox({text("Create user")|bold|color(Color::Cyan)|hcenter,separator(),hbox({vbox({text("Name")|color(Color::Yellow),ni->Render()|border,text("Goal")|color(Color::Yellow),gm->Render()|border,text("Level")|color(Color::Yellow),lm->Render()|border})|flex,vbox({text("Days")|color(Color::Yellow),dm->Render()|border,text("Duration")|color(Color::Yellow),um->Render()|border,text("Limitations")|color(Color::Yellow),xi->Render()|border})|flex}),ms.empty()?text(""):text(ms)|color(Color::Red)|hcenter,hbox({sv->Render(),text(" "),bk->Render()})|hcenter})|border;});
sc.Loop(rd);
}
void view(){
auto sc=ScreenInteractive::TerminalOutput();
std::string n,ms;
std::optional<User> u;
auto ni=Input(&n,"name");
auto se=Button("Search",[&]{u=find(n);ms=u?"":"Not found in client cache";});
auto bk=Button("Back",sc.ExitLoopClosure());
auto ct=Container::Vertical({ni,Container::Horizontal({se,bk})});
auto rd=Renderer(ct,[&]{
Elements r;
r.push_back(text("View user")|bold|color(Color::Cyan)|hcenter);
r.push_back(separator());
r.push_back(ni->Render()|border);
r.push_back(hbox({se->Render(),text(" "),bk->Render()})|hcenter);
if(!ms.empty())r.push_back(text(ms)|color(Color::Red)|hcenter);
if(u)r.push_back(vbox({hbox({text("Name: ")|color(Color::Yellow),text(u->n)}),hbox({text("Goal: ")|color(Color::Yellow),text(u->g)}),hbox({text("Level: ")|color(Color::Yellow),text(u->l)}),hbox({text("Days: ")|color(Color::Yellow),text(std::to_string(u->d))}),hbox({text("Minutes: ")|color(Color::Yellow),text(std::to_string(u->m))}),hbox({text("Limitations: ")|color(Color::Yellow),text(u->x)})})|border);
return vbox(r)|border;
});
sc.Loop(rd);
}
void plan(){
auto sc=ScreenInteractive::TerminalOutput();
std::string n,rs;
auto ni=Input(&n,"name");
auto se=Button("Get plan",[&]{rs=net.cmd("GET_PLAN "+esc(n));msg=rs;});
auto bk=Button("Back",sc.ExitLoopClosure());
auto ct=Container::Vertical({ni,Container::Horizontal({se,bk})});
auto rd=Renderer(ct,[&]{return vbox({text("Workout plan")|bold|color(Color::Cyan)|hcenter,separator(),ni->Render()|border,hbox({se->Render(),text(" "),bk->Render()})|hcenter,rs.empty()?text(""):paragraph(rs)|border})|border;});
sc.Loop(rd);
}
void stat(){msg=net.cmd("SERVER_STATUS");}
void login(){
auto sc=ScreenInteractive::TerminalOutput();
std::string u,p,rs;
auto ui=Input(&u,"admin or member");
auto pi=Input(&p,"password");
auto ok=Button("Login",[&]{rs=net.cmd("LOGIN "+u+" "+p);msg=rs;if(rs.rfind("OK",0)==0)sc.ExitLoopClosure()();});
auto bk=Button("Back",sc.ExitLoopClosure());
auto ct=Container::Vertical({ui,pi,Container::Horizontal({ok,bk})});
auto rd=Renderer(ct,[&]{return vbox({text("Login")|bold|color(Color::Cyan)|hcenter,separator(),text("Username")|color(Color::Yellow),ui->Render()|border,text("Password")|color(Color::Yellow),pi->Render()|border,hbox({ok->Render(),text(" "),bk->Render()})|hcenter,rs.empty()?text(""):text(rs)|color(Color::Red)|hcenter})|border;});
sc.Loop(rd);
}
};
int main(){
App a;
a.run();
return 0;
}
