    #def FEATURE_ENABLED     

!int <64> main()
{
    #ifdef   FEATURE_ENABLED    	 
        !int <64> feature_val = 100;
      #else   
        !int <64> feature_val = 0;
        #endif   

    ret feature_val;
}

: expected exit code : 100