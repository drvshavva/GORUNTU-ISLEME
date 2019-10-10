
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#include <math.h>

//filtre matrisinin size degeri
#define SIZE 3

//PGM resim için  structure olusturdum
typedef struct _PGMInfo {
    int row; //satir bilgisi
    int col; //sütün bilgisi
    int max_gray;  //max gray bilgisi
    int **matrix;  //pixel bilgisi için matrix 
} PGMInfo;

//fonksiyonlar -----------------------------
// matris icin allocation ve deallocation yapan fonksiyonlar 

//allocation
int **allocate_matrix(int row, int col)
{
	int **val;
	int i;

	val = (int **)malloc(sizeof(int *) * row);
	if (val == NULL) {
               printf("allocation yapilamadi..\n");
               exit(1);
	}

	for (i = 0; i < row; ++i) {
		val[i] = (int *)malloc(sizeof(int) * col);
		if (val[i] == NULL) {
			printf("allocation yapilamadi..\n");
               exit(1);
		}
	}
     return val;
}

//deallocation
void deallocate_matrix(int **matrix, int row)
{
	int i;

	for (i = 0; i < row; ++i) {
		free(matrix[i]);
	}
	free(matrix);
}

//pgm dosyasindaki yorum satirlarini gecmek icin yazilan  yorumgeç fonksiyonu
void YorumGec(FILE *f)
{
	int ch;
	char line[100];
	//bu fonksiyonda amaçlanan pgm dosyasindaki yorum satirlarini geçmek 
	//bu fonksiyon cagrildiginda yorum satiri varsa onu geciyor ve yorum satiri bitince tekrar kontrol ediyor var mi diye
	//sonrasinda eger yorum satiri yoksa dosya konum isaretsinin konumunu belirliyor yani okumaya kaldigi yere geri döndürüyor
	
	// okuma yapiyor eger bosluk karakterine denk gelirse bosluk karakterleri bitene
	//kadar dosyadan okuma yapiyor-yani bosluklari geciyor-ASCII
	while ((ch = fgetc(f)) != EOF && isspace(ch)) {
		;
	}

     //eger okudugu karakter komment isareti ise o satiri geciyor 
	if (ch == '#') {
		fgets(line, sizeof(line), f);
		YorumGec(f);
	} else {
		//eger bosluk karakteri degilse dosya konum göstericisinin yerini bir karakter geriye aliyor okumaya devam etsin diye
		fseek(f, -1, SEEK_CUR);
	}
} 

//PGM dosyasini okuma fonksiyonu 
PGMInfo* readPGM(const char *filename, PGMInfo *data)
{
	FILE *pgm;
	char version[3];
	int i, j;

	//dosyamizi okumak icin binary modda aciyoruz 
	if ((	pgm = fopen(filename, "rb")) == NULL) {
		printf("okumak icin %s dosyasi acilamadi!!\n",filename);
		exit(1);
	}
	
	//pgm versiyonunu pgm dosyasindan aliyoruz , ilk satirindan
	fgets(version, sizeof(version), pgm);
	
	//pgm dosyasindan pixel bilgilerini aliyoruz ve max gri bilgisini
	YorumGec(pgm);
	fscanf(pgm, "%d", &data->col);
	YorumGec(pgm);
	fscanf(pgm, "%d", &data->row);
	YorumGec(pgm);
	fscanf(pgm, "%d", &data->max_gray);
	fgetc(pgm);

    //matrix olusturuyoruz pixel bilgisi icin
	data->matrix = allocate_matrix(data->row, data->col);
	
		for (i = 0; i < data->row; ++i) {
			for (j = 0; j < data->col; ++j) {
				data->matrix[i][j] = fgetc(pgm);
			}
		}
		
	fclose(pgm);
	return data;

}

//PGM yazmak icin fonksiyon 
void writePGM(const char *filename, const PGMInfo *data)
{
	FILE *pgmFile;
	int i, j;
   
    //binary modda yazmak icin dosya aciyoruz
	if ((pgmFile = fopen(filename, "wb")) == NULL) {
		printf("yazmak icin %s dosyasi acilamadi !!\n",filename);
		exit(1);
	}

     //PGM header bilgilerini ekliyoruz ilk önce 
	fprintf(pgmFile, "P5 ");
	fprintf(pgmFile, "%d %d ", data->col, data->row);
	fprintf(pgmFile, "%d ", data->max_gray);


		for (i = 0; i < data->row; ++i) {
			for (j = 0; j < data->col; ++j) {
			    fputc((data->matrix[i][j]), pgmFile);
			}
		}
	fclose(pgmFile);
	deallocate_matrix(data->matrix, data->row);
}

//GAUSS filtresi olusturan fonksiyon
void GaussFilter(double Gauss[3][3]) 
{ 
    // standart sapma degerinin karesi olan degeri 2 aldik  
    double sigma_kare = 2; 
    double r, s = 2.0 * sigma_kare; 
  
    // normalizasyon icin bir sum degiskeni
    //buldugumuz gauss filtre degerlerini topluyor sonra
    //bütün elemanlari bu toplam degerine bolcem, daha kucuk degerler elde etmek icin 
    double sum = 0.0; 
  
    // 3*3 lik bir gauss filtresi 
    for (int x = -1; x <= 1; x++) { 
        for (int y = -1; y <= 1; y++) { 
            r = x * x + y * y; 
            Gauss[x + 1][y + 1] = (exp(-r/s)) / (M_PI * s); 
            sum += Gauss[x + 1][y + 1]; 
        } 
    } 
  
    //normalize ediyoruz filtre degerlerini 
    printf("uygulanan gauss filtresi:\n");
    for (int i = 0; i < 3; ++i) { 
	    for (int j = 0; j < 3; ++j){ 
		     Gauss[i][j] /= sum; 
		     printf("%f\t",Gauss[i][j]);
		} 
		printf("\n");
	}
       
           
} 
   //Konvolüsyon fonksiyonu ,reim ,maske ve maskenin boyutu ile geliyor
PGMInfo * Convolution(PGMInfo* image,double mask[][SIZE] ,int m){
  int i,j,k,l,sum=0;
  PGMInfo *result; //konvolüsyon islemi sonucu döndürelecek pgm resim
  
  result = (PGMInfo *) malloc(sizeof(PGMInfo));
  
  //sütün ve satir sayisi resminki ile ayni
  result->col = image->col;  
  result->row = image->row;
  
  //max grey 255 olarak belirledik
  result->max_gray= 255;
  result->matrix = allocate_matrix(image->row, image->col);
  
  int image_size = image -> row * image -> col;
  
  for(i=1;i<=image->row;i++){
  	for(j=1;j<=image->col;j++){
  		//konvolüsyon uygulanan pixel indisi
  		//konvolüsyon maskenin sol üst köseden baslayarak görüntü üzerine konulur
  		//daha sonra her piksel degerine karsilik gelen maske degeriyle çarpilip tüm sonuçlarin toplanmasi
  		//ve toplanan bu degerin yeni matris degeri olmasi seklinde yapilmistir
  		int img_ind = image -> col * i + j;
  		sum=0;
  		
  		//maske/filtre uygulaniyor
  		for(k=1;k<=m;k++){
  			for(l=1;l<m;l++){
  				int mask_ind = m * k + l;
  				//resmin matrixinin disina cikip cikmadigini kontrol ediyor
  				if(img_ind + mask_ind < image_size && img_ind+image->col*k+l <image_size){
  				     sum+=(image->matrix[i+k-1][j+l-1]*mask[k][l]);
  				}
			  }
		  }
		  
		int res_i = img_ind + (image->col * (m/2))+ m/2;
		if(res_i < image_size) {
		   result->matrix[i+(m/2)][j+(m/2)] = sum;
	    }
	  }
	  printf("\n");
  }
   return(result);
}
        

//ana fonksiyon
int main() {
	//konvolusyon islemi icin  pgmler
  PGMInfo* conv_img_gauss;
  PGMInfo* conv_img_sobel_h;
  PGMInfo* conv_img_laplacian;
  PGMInfo* conv_img_sobel_v;
  PGMInfo* image_g;
  PGMInfo* image;
  
  int m;//filtre maske boyutu icin
  char filename[256]; 
  double Gauss[3][3];
  

   printf ("Resim dosya adi giriniz: ");
   scanf ("%s", filename);
  

  //allocate memory pgm struct icin
  image = (PGMInfo *) malloc (sizeof(PGMInfo));
  image_g= (PGMInfo *) malloc (sizeof(PGMInfo));
  //resmi oku
  image = readPGM(filename, image);

  //Gauss uygula
  m=3;//3*3 lük filtre
  GaussFilter(Gauss);
  conv_img_gauss = Convolution( image,  Gauss , m  ); 
  writePGM("Gauss.pgm",conv_img_gauss);
  
  //Gausslu resme Sobel fiLtresi uygula
  m=3;//3*3 lük filtre
  double sobel_h[3][3] = {-1, -2, -1, 0, 0, 0, 1, 2, 1}; //yatay
  double sobel_v[3][3] = {-1, 0, 1, -2, 0, 2, -1, 0, 1}; //dikey
  image_g=readPGM("Gauss.pgm",image_g);
  conv_img_sobel_h = Convolution(image_g,sobel_h,m);
  conv_img_sobel_v = Convolution(conv_img_sobel_h, sobel_v , m);
  writePGM("Sobel.pgm",conv_img_sobel_v);
  
  //laplacian filter 
  m=3;
  double laplacian[3][3]={0,1,0,1,-4,1,0,1,0};
  conv_img_laplacian = Convolution(image ,laplacian ,m );
  writePGM("Laplacian.pgm",conv_img_laplacian);
  
  //all done
  printf("Islem tamamlandi...\n\n");
  system("pause");
  return -1;
}
