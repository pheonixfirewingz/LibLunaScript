namespace LunaScriptEXT.src.Client
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using Microsoft.VisualStudio.Text;
    using Microsoft.VisualStudio.Text.Tagging;
    using Microsoft.VisualStudio.Utilities;

    [Export(typeof(ITaggerProvider))]
    [ContentType("LunaScript")]
    [TagType(typeof(LunaScriptTag))]
    internal sealed class LunaScriptTagProvider : ITaggerProvider
    {

        public ITagger<T> CreateTagger<T>(ITextBuffer buffer) where T : ITag
        {
            return new LunaScriptTagger(buffer) as ITagger<T>;
        }
    }

    public class LunaScriptTag : ITag
    {
        public LunaScript.Share.Constants.WORD_TYPE type { get; private set; }
        public LunaScriptTag(LunaScript.Share.Constants.WORD_TYPE type_in)
        {
            type = type_in;
        }
    }

    internal sealed class LunaScriptTagger : ITagger<LunaScriptTag>
    {

        ITextBuffer buffer;
        readonly IDictionary<string, LunaScript.Share.Constants.WORD_TYPE> types = new Dictionary<string, LunaScript.Share.Constants.WORD_TYPE>();

        internal LunaScriptTagger(ITextBuffer buffer_in)
        {
            buffer = buffer_in;
            types["void"] = LunaScript.Share.Constants.WORD_TYPE.VOID;
            types["int8"] = LunaScript.Share.Constants.WORD_TYPE.INT8;
            types["int16"] = LunaScript.Share.Constants.WORD_TYPE.INT16;
            types["int32"] = LunaScript.Share.Constants.WORD_TYPE.INT32;
            types["int64"] = LunaScript.Share.Constants.WORD_TYPE.INT64;
            types["uint8"] = LunaScript.Share.Constants.WORD_TYPE.UINT8;
            types["uint16"] = LunaScript.Share.Constants.WORD_TYPE.UINT16;
            types["uint32"] = LunaScript.Share.Constants.WORD_TYPE.UINT32;
            types["uint64"] = LunaScript.Share.Constants.WORD_TYPE.UINT64;
            types["float32"] = LunaScript.Share.Constants.WORD_TYPE.FLOAT32;
            types["float64"] = LunaScript.Share.Constants.WORD_TYPE.FLOAT64;
            types["func"] = LunaScript.Share.Constants.WORD_TYPE.FUNC;
            types["public"] = LunaScript.Share.Constants.WORD_TYPE.PUBLIC;
            types["ret"] = LunaScript.Share.Constants.WORD_TYPE.RET;
            types["and"] = LunaScript.Share.Constants.WORD_TYPE.AND;
            types["or"] = LunaScript.Share.Constants.WORD_TYPE.OR;
            types["mod"] = LunaScript.Share.Constants.WORD_TYPE.MOD;
            types["xor"] = LunaScript.Share.Constants.WORD_TYPE.XOR;
            types["global"] = LunaScript.Share.Constants.WORD_TYPE.GLOBAL;
            types["import"] = LunaScript.Share.Constants.WORD_TYPE.IMPORT;
            types["module"] = LunaScript.Share.Constants.WORD_TYPE.MODULE;
            types["->"] = LunaScript.Share.Constants.WORD_TYPE.SUPER;
            types["{"] = LunaScript.Share.Constants.WORD_TYPE.L_SQUIGGLY;
            types["}"] = LunaScript.Share.Constants.WORD_TYPE.R_SQUIGGLY;
        }

        public event EventHandler<SnapshotSpanEventArgs> TagsChanged
        {
            add { }
            remove { }
        }

        public IEnumerable<ITagSpan<LunaScriptTag>> GetTags(NormalizedSnapshotSpanCollection spans)
        {
            foreach (SnapshotSpan curSpan in spans)
            {
                ITextSnapshotLine containingLine = curSpan.Start.GetContainingLine();
                int curLoc = containingLine.Start.Position;
                string[] tokens = containingLine.GetText().ToLower().Split(' ');

                foreach (string t in tokens)
                {
                    if (types.ContainsKey(t))
                    {
                        var tokenSpan = new SnapshotSpan(curSpan.Snapshot, new Span(curLoc, t.Length));
                        if (tokenSpan.IntersectsWith(curSpan))
                            yield return new TagSpan<LunaScriptTag>(tokenSpan, new LunaScriptTag(types[t]));
                    }
                    curLoc += t.Length + 1;
                }
            }

        }
    }
}