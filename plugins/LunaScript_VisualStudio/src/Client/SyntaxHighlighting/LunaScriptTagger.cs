using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Text;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Tagging;
using Microsoft.VisualStudio.Utilities;

namespace LunaScriptEXT.src.Client
{
    [Export(typeof(ITaggerProvider))]
    [ContentType("LunaScript")]
    [TagType(typeof(LunaScriptTag))]
    internal sealed class LunaScriptTagProvider : ITaggerProvider
    {
        public ITagger<T> CreateTagger<T>(ITextBuffer buffer) where T : ITag
        {
            return new LunaScriptTagger() as ITagger<T>;
        }
    }

    public class LunaScriptTag : ITag
    {
        public string token { get; private set; }
        public bool isKeyword { get; private set; }
        public LunaScriptTag(bool isKeywordIn, string tokenIn)
        {
            isKeyword = isKeywordIn;
            token = tokenIn;
        }
    }

    internal sealed class LunaScriptTagger : ITagger<LunaScriptTag> 
    {
        readonly IDictionary<string, bool> types = new Dictionary<string, bool>();
        internal LunaScriptTagger()
        {
            types["void"] = true;
            types["int8"] = true;
            types["int16"] = true;
            types["int32"] = true;
            types["int64"] = true;
            types["uint8"] = true;
            types["uint16"] = true;
            types["uint32"] = true;
            types["uint64"] = true;
            types["float32"] = true;
            types["float64"] = true;
            types["func"] = true;
            types["public"] = true;
            types["ret"] = true;
            types["and"] = true;
            types["or"] = true;
            types["mod"] = true;
            types["xor"] = true;
            types["global"] = true;
            types["import"] = true;
            types["module"] = true;
            types["->"] = false;
            types["{"] = false;
            types["}"] = false;
            types[")"] = false;
            types["("] = false;
            types["+"] = false;
            types["-"] = false;
            types["*"] = false;
            types["/"] = false;
            types["="] = false;
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
                string lineText = containingLine.GetText().ToLower();
                List<string> tokens = new List<string>();
                StringBuilder sb = new StringBuilder();

                for (int i = 0; i < lineText.Length; i++)
                {
                    char c = lineText[i];

                    if (Char.IsLetterOrDigit(c))
                        sb.Append(c);
                    else if (c == '_' && sb.Length > 0 && Char.IsLetterOrDigit(sb[sb.Length - 1]))
                        sb.Append(c);
                    else
                    {
                        if (sb.Length > 0)
                        {
                            tokens.Add(sb.ToString());
                            sb.Clear();
                        }

                        if (!Char.IsWhiteSpace(c))
                        {
                            if (c == '-' && i + 1 < lineText.Length && lineText[i + 1] == '>')
                            {
                                tokens.Add("->");
                                i++;
                            }
                            else
                                tokens.Add(c.ToString());
                        }
                        else
                            tokens.Add(c.ToString());
                    }
                }

                if (sb.Length > 0)
                    tokens.Add(sb.ToString());

                foreach (string token in tokens)
                {
                    if (types.ContainsKey(token))
                    {
                        var tokenSpan = new SnapshotSpan(curSpan.Snapshot, new Span(curLoc, token.Length));
                        if (tokenSpan.IntersectsWith(curSpan))
                            yield return new TagSpan<LunaScriptTag>(tokenSpan, new LunaScriptTag(types[token],token));
                    }
                    curLoc += token.Length;
                }
            }
        }
    }
}