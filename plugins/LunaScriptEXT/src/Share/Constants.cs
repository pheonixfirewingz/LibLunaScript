namespace LunaScript.Share
{
    using Color = System.Windows.Media.Color;
    using Colors = System.Windows.Media.Colors;

    public static class Constants
    {
        public enum WORD_TYPE
        {
            VOID,
            INT8,
            INT16,
            INT32,
            INT64,
            UINT8,
            UINT16,
            UINT32,
            UINT64,
            FLOAT32,
            FLOAT64,
            FUNC,
            PUBLIC,
            RET,
            AND,
            OR,
            MOD,
            XOR,
            GLOBAL,
            IMPORT,
            MODULE,
            R_SQUIGGLY,
            L_SQUIGGLY,
            SUPER,
        };

        public static Color GetCurrentThemeSymbolColor()
        {
            //TODO: get syntax colour from the users theme
            return Colors.Red;
        }

        public static Color GetCurrentThemeKeywordColor()
        {
            //TODO: get syntax colour from the users theme
            return Colors.DarkCyan;
        }
    }
}
