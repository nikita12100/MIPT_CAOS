
typedef enum {
    PlusZero      = 0x00,
    MinusZero     = 0x01,
    PlusInf       = 0xF0,
    MinusInf      = 0xF1,
    PlusRegular   = 0x10,
    MinusRegular  = 0x11,
    PlusDenormal  = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN  = 0x30,
    QuietNaN      = 0x31
} float_class_t;

extern float_class_t classify(double *value_ptr){
    void* a_ptr_as_void = value_ptr;
    unsigned long long* a_ptr_as_uint = a_ptr_as_void;
    unsigned long long b = *a_ptr_as_uint;

    char mantisa[52];
    char exp[11];
    char s;
    for(int i = 52; i > 0; i--){
        if((b & 1) == 1) // 1
            mantisa[i] = '1';
        else    // 0
            mantisa[i] = '0';
        b >>= 1;
    }
    for(int i = 11; i > 0; i--){
        if((b & 1) == 1)
            exp[i] = '1';
        else
            exp[i] = '0';
        b >>= 1;
    }
    if((b & 1) == 1)
        s = '1';
    else
        s = '0';

    char is_m_zero = 1; // true
    for(int i = 1; i <= 52; i++)
        if(mantisa[i] == '1')
            is_m_zero = 0;

    char is_e_zero = 1; // true
    for(int i = 1; i <= 11; i++)
        if(exp[i] == '1')
            is_e_zero = 0;

    char is_e_one = 1;
    for(int i = 1; i <= 11; i++)
        if(exp[i] == '0')
            is_e_one = 0;

    if(is_e_one == 1 && is_m_zero == 0)
        return SignalingNaN;
    if(is_e_one == 1 && is_m_zero == 1)
        return QuietNaN;

    if(is_e_zero == 1 && is_m_zero == 1 && s == '0')
        return PlusZero;
    if(is_e_zero == 1 && is_m_zero == 1 && s == '1')
        return MinusZero;
    if(is_e_zero == 0 && is_m_zero == 1 && s == '0')
        return PlusInf;
    if(is_e_zero == 0 && is_m_zero == 1 && s == '1')
        return MinusInf;
    if(is_e_zero == 0 && is_m_zero == 0 && s == '0')
        return PlusRegular;
    if(is_e_zero == 0 && is_m_zero == 0 && s == '1')
        return MinusRegular;
    if(is_e_zero == 1 && is_m_zero == 0 && s == '0')
        return PlusDenormal;
    if(is_e_zero == 1 && is_m_zero == 0 && s == '1')
        return MinusDenormal;

    return PlusRegular;
}