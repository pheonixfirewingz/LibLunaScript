using System.ComponentModel.Composition;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Utilities;

namespace LunaScriptEXT.src.Client
{
    internal static class SyntaxDefinitions
    {
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("void")]
        internal static ClassificationTypeDefinition voidExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("int8")]
        internal static ClassificationTypeDefinition int8Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("int16")]
        internal static ClassificationTypeDefinition int16Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("int32")]
        internal static ClassificationTypeDefinition int32Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("int64")]
        internal static ClassificationTypeDefinition int64Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("uint8")]
        internal static ClassificationTypeDefinition uint8Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("uint16")]
        internal static ClassificationTypeDefinition uint16Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("uint32")]
        internal static ClassificationTypeDefinition uint32Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("uint64")]
        internal static ClassificationTypeDefinition uint64Exclamation = null;        
        
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("float32")]
        internal static ClassificationTypeDefinition float32Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("float64")]
        internal static ClassificationTypeDefinition float64Exclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("func")]
        internal static ClassificationTypeDefinition funcExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("public")]
        internal static ClassificationTypeDefinition publicExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("ret")]
        internal static ClassificationTypeDefinition retExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("and")]
        internal static ClassificationTypeDefinition andExclamation = null;        
        
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("or")]
        internal static ClassificationTypeDefinition orExclamation = null;        
        
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("mod")]
        internal static ClassificationTypeDefinition modExclamation = null;
        
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("xor")]
        internal static ClassificationTypeDefinition xorExclamation = null;
        
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("global")]
        internal static ClassificationTypeDefinition globalExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("import")]
        internal static ClassificationTypeDefinition importExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("module")]
        internal static ClassificationTypeDefinition moduleExclamation = null;        
        
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("->")]
        internal static ClassificationTypeDefinition superExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("{")]
        internal static ClassificationTypeDefinition lSquigallyExclamation = null;

        [Export(typeof(ClassificationTypeDefinition))]
        [Name("}")]
        internal static ClassificationTypeDefinition rSquigallyExclamation = null;
    }
}