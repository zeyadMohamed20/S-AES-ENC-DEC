/************************************ INCLUDES ************************************************************/
#include <stdio.h>

/************************************ MACRO DECLARATION ***************************************************/
#define TRUE 1
#define FALSE 0
/************************************ DATA TYPE DECLARATION ***********************************************/
typedef unsigned char boolean;

typedef unsigned char uint8;

typedef unsigned short uint16;

typedef unsigned long uint32;

typedef unsigned long long uint64;

typedef signed char sint8;

typedef signed short sint16;

typedef signed long sint32;

typedef signed long long sint64;

typedef float float32;

typedef double float64;

typedef struct
{
    uint16 key0;
    uint16 key1;
    uint16 key2;

}Round_Keys;

/************************************ Function DECLARATIONS *************************************/
int areStringsEqual(const char *str1, const char *str2);
int hexCharToInt(char hexChar);
int hexStringToInt(const char *hexString) ;

uint8 rotate_nibble(uint8 word);
Round_Keys generate_Keys(uint16 key);
uint16 add_roundKey(uint16 text,uint16 key);
uint16 shift_row(uint16 text);
uint8 substitute_nibble(uint8 word);
uint16 mix_column(uint16 text);
uint8 inverse_substitute_nibble(uint8 word);
uint16 inverse_mix_column(uint16 text);

uint16 encrypt(uint16 plainText,uint16 key);
uint16 decrypt(uint16 cipherText,uint16 key);

/************************************ Function Definitions *************************************/
int main(int argc, char *argv[])
{
	uint16 plainText = 0xD728;
	uint16 key = 0x4AF5;
	uint16 cipherText = 0x24EC;

	if(areStringsEqual(argv[1],"ENC"))
	{
		plainText = hexStringToInt(argv[3]);
		key = hexStringToInt(argv[2]);
		cipherText = encrypt(plainText,key);
		printf("\nPerforming Encryption.......\n");
		printf("PlainText: %X\t\t",plainText);
		printf("CipherText: %X\n",cipherText);
	}
	else if(areStringsEqual(argv[1],"DEC"))
	{
		cipherText = hexStringToInt(argv[3]);
		key = hexStringToInt(argv[2]);
		plainText = decrypt(cipherText,key);
		printf("\nPerforming Decryption.......\n");
		printf("CipherText: %X\t\t",cipherText);
		printf("PlainText: %X\n",plainText);
	}
}

// Function to compare two strings
int areStringsEqual(const char *str1, const char *str2) {
    while (*str1 != '\0' && *str2 != '\0') {
        if (*str1 != *str2) {
            return 0;  // Not equal
        }
        str1++;
        str2++;
    }

    // Check if both strings reached the end simultaneously
    return *str1 == '\0' && *str2 == '\0';
}

// Function to convert a hexadecimal character to its integer equivalent
int hexCharToInt(char hexChar) 
{
    if (hexChar >= '0' && hexChar <= '9') 
	{
        return hexChar - '0';
    } else if (hexChar >= 'a' && hexChar <= 'f') 
	{
        return hexChar - 'a' + 10;
    } else if (hexChar >= 'A' && hexChar <= 'F') 
	{
        return hexChar - 'A' + 10;
    } else {
        // Invalid hexadecimal character
        return -1;
    }
}

// Function to convert a hexadecimal string to an integer
int hexStringToInt(const char *hexString) 
{
    int result = 0;
    int index = 0;

    // Handle the "0x" prefix if present
    if (hexString[0] == '0' && (hexString[1] == 'x' || hexString[1] == 'X')) 
	{
        index = 2;
    }

    while (hexString[index] != '\0') 
	{
        int digitValue = hexCharToInt(hexString[index]);

        if (digitValue == -1) {
            // Invalid hexadecimal character
            return -1;
        }

        result = result * 16 + digitValue;
        index++;
    }

    return result;
}

uint8 rotate_nibble(uint8 word)
{
    uint8 nibble[2] = {0};
    nibble[0] = (word&(0xF0))>>4;
    nibble[1] = (word&(0x0F))<<4;
    word = nibble[1] | nibble[0];
    return word;
}


Round_Keys generate_Keys(uint16 key)
{
    Round_Keys generatedKeys;
    const uint8 Rcon[2] = {0x80,0x30};
    uint8 word[6] = {0};
    word[0] = (key&0xFF00)>>8;
    word[1] = key&0x00FF;
    word[2] = word[0]^Rcon[0]^substitute_nibble(rotate_nibble(word[1]));
    word[3] = word[2]^word[1];
    word[4] = word[2]^Rcon[1]^substitute_nibble(rotate_nibble(word[3]));
    word[5] = word[4]^word[3];
    generatedKeys.key0 = (word[0]<< 8) |word[1];
    generatedKeys.key1 = ((short int)word[2] << 8)|word[3];
    generatedKeys.key2 = ((short int)word[4] << 8)|word[5];
    return generatedKeys;
}


uint16 add_roundKey(uint16 text,uint16 key)
{
	return text^key;
}

uint16 shift_row(uint16 text)
{
	return (text & 0xF000) | (text & 0x000F)<<8 | (text & 0x00F0) | (text & 0x0F00)>>8;
}

uint8 substitute_nibble(uint8 word)
{
    char s_box[4][4] = {
                        {0X09,0X04,0X0A,0X0B},
                        {0x0D,0x01,0x08,0x05},
                        {0x06,0x02,0x00,0x03},
                        {0x0C,0x0E,0x0F,0x07}
                        };
    word = ((s_box[(word&0xC0)>>6][(word&0x30)>>4])<<4) | (s_box[(word&0x0C)>>2][word&0x03]);

    return word;
}

uint16 mix_column(uint16 text)
{
	uint8 multiplyingResult[15] = {0x4,0x8,0xC,0x3,0x7,0xB,0xF,0x6,0x2,0xE,0xA,0x5,0x1,0xD,0x9};

	uint8 nibble1Old = (text&0xF000)>>12;
	uint8 nibble2Old = (text&0x0F00)>>8;
	uint8 nibble3Old = (text&0x00F0)>>4;
	uint8 nibble4Old = (text&0x000F);

	uint8 nibble1New = nibble1Old ^ multiplyingResult[nibble2Old-1];
	uint8 nibble2New = nibble2Old ^ multiplyingResult[nibble1Old-1];
	uint8 nibble3New = nibble3Old ^ multiplyingResult[nibble4Old-1];
	uint8 nibble4New = nibble4Old ^ multiplyingResult[nibble3Old-1];

	uint16 result = (nibble1New << 12) | (nibble2New << 8) | (nibble3New << 4) | (nibble4New);

	return result;
}

uint8 inverse_substitute_nibble(uint8 word)
{
    char s_box[4][4] = {
                        {0X0A,0X05,0X09,0X0B},
                        {0x01,0x07,0x08,0x0F},
                        {0x06,0x00,0x02,0x03},
                        {0x0C,0x04,0x0D,0x0E}
                        };
    word = ((s_box[(word&0xC0)>>6][(word&0x30)>>4])<<4) | (s_box[(word&0x0C)>>2][word&0x03]);

    return word;
}

uint16 inverse_mix_column(uint16 text)
{
	uint16 multiplyingResult[2][15] = {
										{0x2,0x4,0x6,0x8,0xA,0xC,0xE,0x3,0x1,0x7,0x5,0xB,0x9,0xF,0xD},
										{0x9,0x1,0x8,0x2,0xB,0x3,0xA,0x4,0xD,0x5,0xC,0x6,0xF,0x7,0xE}
		                              };
	uint8 nibble1Old = (text&0xF000)>>12;
	uint8 nibble2Old = (text&0x0F00)>>8;
	uint8 nibble3Old = (text&0x00F0)>>4;
	uint8 nibble4Old = (text&0x000F);

	uint8 nibble1New = multiplyingResult[1][nibble1Old-1] ^ multiplyingResult[0][nibble2Old-1];
	uint8 nibble2New = multiplyingResult[0][nibble1Old-1] ^ multiplyingResult[1][nibble2Old-1];
	uint8 nibble3New = multiplyingResult[1][nibble3Old-1] ^ multiplyingResult[0][nibble4Old-1];
	uint8 nibble4New = multiplyingResult[0][nibble3Old-1] ^ multiplyingResult[1][nibble4Old-1];

	uint16 result = (nibble1New << 12) | (nibble2New << 8) | (nibble3New << 4) | (nibble4New);

	return result;
}

uint16 encrypt(uint16 plainText,uint16 key)
{
	Round_Keys generatedKeys = generate_Keys(key);
	uint16 result = add_roundKey(plainText,generatedKeys.key0);
	result = (substitute_nibble((result & 0xFF00)>>8)<<8) | (substitute_nibble(result & 0x00FF));
	result = shift_row(result);
	result = mix_column(result);
	result = add_roundKey(result,generatedKeys.key1);
	result = (substitute_nibble((result & 0xFF00)>>8)<<8) | (substitute_nibble(result & 0x00FF));
	result = shift_row(result);
	result = add_roundKey(result,generatedKeys.key2);
	return result;
}

uint16 decrypt(uint16 cipherText,uint16 key)
{
	Round_Keys generatedKeys = generate_Keys(key);
	uint16 result = add_roundKey(cipherText,generatedKeys.key2);
	result = shift_row(result);
	result = (inverse_substitute_nibble((result & 0xFF00)>>8)<<8) | (inverse_substitute_nibble(result & 0x00FF));
	result = add_roundKey(result,generatedKeys.key1);
	result = inverse_mix_column(result);
	result = shift_row(result);
	result = (inverse_substitute_nibble((result & 0xFF00)>>8)<<8) | (inverse_substitute_nibble(result & 0x00FF));
	result = add_roundKey(result,generatedKeys.key0);
	return result;
}
