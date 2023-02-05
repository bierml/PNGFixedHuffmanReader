#include <windows.h>         // Подключаем заголовочный файл Windows
#include <wingdi.h>
#include "PNGFormat.h"
#include <stdbool.h>
//----------------------------------------------------------------------------------------
// Функция
// MainWinProc()
// Здесь происходит обработка сообщенний
//----------------------------------------------------------------------------------------
pixel_ * pixarr;
unsigned char* bytethread_;
IHDR_ InfoHeader;
LRESULT CALLBACK MainWinProc(HWND hwnd,             //Дескриптор окна
                            UINT msg,               //идентефикатор сообщение
                            WPARAM wparam,          //Дополнительная информация
                            LPARAM lparam)          //Дополнительная информация
{
HDC hDC_;
void* ps;
switch(msg)
    {
    case WM_PAINT:
        {
            hDC_ = GetDC(hwnd);
            HBRUSH hBrush,hOldBrush; //создаём объект-кисть
            RECT r;
            for(int i = 0;i<InfoHeader.height;i++)
            {
                for(int j=0;j<InfoHeader.width;j++)
                {
                     r.left = 25*j;
                     r.top = 25*i;
                     r.right = 25*(j+1);
                     r.bottom = 25*(i+1);
                     hBrush = CreateSolidBrush(RGB(pixarr[i*InfoHeader.width+j].red_,pixarr[i*InfoHeader.width+j].green_,pixarr[i*InfoHeader.width+j].blue_)); //задаём сплошную кисть, закрашенную цветом RGB
                     hOldBrush = SelectObject(hDC_,hBrush);
                     FillRect(hDC_, &r, hBrush);
                     SelectObject(hDC_,hOldBrush);
                     DeleteObject(hBrush);
                }
            }
            ReleaseDC(hwnd,hDC_);
            ValidateRect(hwnd, NULL);
            EndPaint(hwnd, ps);
        }
         break;

    case WM_DESTROY:
        {
        PostQuitMessage(0);
        free(pixarr);
        free(bytethread_);
        return(0);
        } break;

    }

  return (DefWindowProc(hwnd, msg, wparam, lparam));
}
//---------------------------------------------------------------------------------------
// Функция
// WinMain()
// Входная точка приложения
//---------------------------------------------------------------------------------------

int WINAPI WinMain( HINSTANCE hinstance,
                    HINSTANCE hprevinstance,
                    LPSTR lpcmdline,
                    int ncmdshow)
{

    WNDCLASSEX windowsclass; // Создем класс
    HWND       hwnd;         // Создаем дескриптор окна
    MSG        msg;          // Сообщение


    // Определим класс окна WNDCLASSEX
    windowsclass.cbSize         = sizeof(WNDCLASSEX);
    windowsclass.style          = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowsclass.lpfnWndProc    = MainWinProc;
    windowsclass.cbClsExtra     = 0;
    windowsclass.cbWndExtra     = 0;
    windowsclass.hInstance      = hinstance;
    windowsclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    windowsclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    windowsclass.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    windowsclass.lpszMenuName   = NULL;
    windowsclass.lpszClassName  = "WINDOWSCLASS";
    windowsclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&windowsclass))
        return(0);
    // Теперь когда класс зарегестрирован можно создать окно
    if (!(hwnd = CreateWindowEx(NULL,              // стиль окна
                            "WINDOWSCLASS",    // класс
                            "PNGViewer",    // название окна
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            0,0,               // левый верхний угол
                            300,300,           // ширина и высота
                            NULL,              // дескриптор родительского окна
                            NULL,              // дескриптор меню
                            hinstance,         // дескриптор экземпляра проложенния
                            NULL)))            // указатель на данные созданного окна
        return(0);
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    FILE *fp;
    def_ch chunk;
    def_ch *ch_pointer;
    zheader head_;
    int position_ = 0;
    int oldposition_ = 0;
    int a,b = 0;
    int numbchunk = 0;
    int n = 0;
    int l = 0;
    int m = 0;
    int i = 0;
    int length_ = 0;
    int distance_ = 0;
    int oldpos;
    int currpos = 3;
    int exbits;
    int rnum;
    int addbitn1 = 0;
    int addbitn2 = 0;
    int ch_counter = 0;
    bool errflag = false;
    int datachnum = -1;
    unsigned long long int filetype = 0;
    char name[] = "myfile_.png";
    if ((fp = fopen(name, "rb")) == NULL)
    {
        MessageBox(hwnd,"Cannot open file!\n","Message",MB_OK);
        return 0;
    }
    fseek(fp,0,SEEK_SET);
    a=fread(&filetype,1,sizeof(unsigned long long int), fp);
    bool flag;
    if(filetype == 0x0A1A0A0D474E5089)
    {
        fread(&InfoHeader,sizeof(IHDR_),1,fp);
        InfoHeader.width = SWAP_UINT32(InfoHeader.width);
        InfoHeader.height = SWAP_UINT32(InfoHeader.height);
        if(InfoHeader.colortype!=6 || InfoHeader.interlace!=0 || InfoHeader.bitdepth!=8)
        {
            MessageBox(hwnd,"Data format error!\nCheck these parameters in your IHDR chunk:\nColortype=6\nBit depth=8\nInterlace=0\n","Message",MB_OK);
            fclose(fp);
            return -4;
        }
        do
        {
            chunk=readchunk(fp);
            freemem(chunk);
            numbchunk++;
            if(chunk.chunkname==SWAP_UINT32(0x49444154))
                datachnum = numbchunk-1;
        }while (chunk.chunkname!=0x444E4549);
        ch_pointer=(def_ch*)malloc(numbchunk*sizeof(def_ch));
        fseek(fp,sizeof(IHDR_)+8,SEEK_SET);
        for(int i=0;i<numbchunk;i++)
            ch_pointer[i]=readchunk(fp);
        pixarr = (pixel_*)malloc(InfoHeader.width*InfoHeader.height*sizeof(pixel_));
        bytethread_ = (unsigned char*)malloc(InfoHeader.width*InfoHeader.height*4*sizeof(unsigned char)+1);
        head_.CMF=ch_pointer[datachnum].chunkdata[0];
        head_.FLG=ch_pointer[datachnum].chunkdata[1];
        if(((head_.FLG) & 32)>>5 == 1)
            n+=4;
        m = 0;
        i=2+n;
        while(i<ch_pointer[datachnum].length)
        {
            flag = true;
            l = 0;
            if(i==2+n)
            {
               errflag=((ch_pointer[datachnum].chunkdata[i] & 6)>>1)!=1;
               errflag=errflag || ((ch_pointer[datachnum].chunkdata[i] & 1)!=1);
            }

            rnum = ch_pointer[datachnum].chunkdata[i]>>currpos;
            rnum=SWAPPER(rnum,7);
            if(currpos!=0 && currpos!=1)
                rnum += SWAPPER((ch_pointer[datachnum].chunkdata[i+1] & MASK(currpos-1)),currpos-1);
            if(addbits(rnum)==1)
            {
                l = 1;
                addbitn1 = (ch_pointer[datachnum].chunkdata[i+1] & MASK(currpos)) >> currpos-1;
                rnum = (rnum << addbits(rnum))+addbitn1;
            }
            if(addbits(rnum)==2)
            {
                l = 2;
                addbitn2 = (ch_pointer[datachnum].chunkdata[i+1] & MASK(currpos)) >> currpos-1;
                addbitn1 = (ch_pointer[datachnum].chunkdata[i+1] & MASK(currpos+1)) >> currpos;
                rnum = (rnum << addbits(rnum))+addbitn2*2+addbitn1;
            }
            if(convertvalue(rnum,l)==256)
                break;
            currpos = currpos+l-1;
            if(convertvalue(rnum,l)>=257 && convertvalue(rnum,l)<=285)
            {
                length_ = tabl3[convertvalue(rnum,l)-257][1];
                exbits = tabl3[convertvalue(rnum,l)-257][0];

                length_ += (ch_pointer[datachnum].chunkdata[i+(currpos>=2)]>>currpos) & MASK(exbits);
                currpos += exbits;

                distance_ = (ch_pointer[datachnum].chunkdata[i+(currpos>=2)]>>currpos) & MASK(5);
                if(8-currpos<5)
                    distance_ += (ch_pointer[datachnum].chunkdata[i+(currpos>=2)+1] & MASK(currpos-3))<<(8-currpos);
                distance_ = SWAPPER(distance_,5);
                exbits = tabl2[distance_][1];
                distance_ = tabl2[distance_][2];
                distance_ += (ch_pointer[datachnum].chunkdata[i+(currpos>=2)+1] >> ((currpos+5) % 8)) & MASK(exbits);
                m = (currpos>3);
                currpos = (currpos+5) % 8+exbits;
                oldposition_ = position_ - distance_;
                for(int s=0;s<length_;s++)
                {
                    bytethread_[position_]=bytethread_[oldposition_];
                    flag = false;
                    oldposition_++;
                    position_++;
                }
            }
            if(flag)
            {
                bytethread_[position_] = convertvalue(rnum,l);
                position_++;
            }
            if(errflag)
            {
                MessageBox(hwnd,"A fatal error has occured!\nCheck these parameters in your PNG file:\nBFINAL=1(only one IDAT chunk)\nBTYPE=01(compressed with fixed Huffman codes)","Message",MB_OK);
                return -2;
            }
            i += m+1;
            m = 0;
        }
        int counter2 = 0;
        for(int counter=0;counter<=(InfoHeader.height*InfoHeader.width*4+4);counter++)
        {
            if(counter !=0 && (counter % (InfoHeader.width*4+1) != 0 || counter==1))
            {
                if(counter2 % 4 == 0)
                   pixarr[counter2 / 4].red_ = bytethread_[counter];
                if(counter2 % 4 == 1)
                   pixarr[counter2/ 4].green_ = bytethread_[counter];
                if(counter2 % 4 == 2)
                    pixarr[counter2 / 4].blue_ = bytethread_[counter];
                if(counter2 % 4 == 3)
                    pixarr[counter2 / 4].alpha_ = bytethread_[counter];
                counter2++;
            }

        }
    }else
        MessageBox(hwnd,"Wrong file format!\n","Message",MB_OK);
    fclose(fp);
    getchar();
    // Зарегестрируем класс
    ShowWindow( hwnd, SW_SHOWDEFAULT );    //Нарисуем окно
    UpdateWindow( hwnd );                  //Обновим окно
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return(msg.wParam);
}
