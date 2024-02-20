#define MULTI

#ifdef MULTI
    // #define IMAGE_PATH {"","/comexp-ikebukuro.png","/exp-kotesashi.png","/exp-seibu-shinjuku.png","/f_rapidexp_motomachi-chukagai.png","/haijima_rapid-kodaira.png","/hijima_liner-seibutachikawa.png","/local-seibushinjuku.png","/rapidexp-hanno.png","/rapid-irumashi.png","/s_train-ginza-itchome.png","/semi_exp-shintokorozawa.png"}
    // #define IMAGE_PATH "/exp-seibu-shinjuku.png"
     #define IMAGE_PATH "/com_semiexp-seibuchichibu.png"
    // #define IMAGE_PATH "/comexp-ikebukuro.png"
    // #define IMAGE_PATH "/local-seibushinjuku.png"

#else
    #define IMAGE_PATH "/com_semiexp-null.png"
#endif