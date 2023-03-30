using System.Collections.Generic;
using Microsoft.VisualStudio.Language.Intellisense;
using Microsoft.VisualStudio.Text;
using System.ComponentModel.Composition;
using Microsoft.VisualStudio.Utilities;
using System.Threading.Tasks;
using System.Threading;
using Microsoft.VisualStudio.Language.StandardClassification;
using Microsoft.VisualStudio.Text.Adornments;
using Microsoft.VisualStudio.Text.Tagging;
using System.Linq;

namespace LunaScriptEXT.src.Client.Intellisense
{
    [Export(typeof(IAsyncQuickInfoSourceProvider))]
    [ContentType("LunaScript")]
    [Name("LunaScriptQuickInfo")]
    internal class QuickInfoProvider : IAsyncQuickInfoSourceProvider
    {
        public IAsyncQuickInfoSource TryCreateQuickInfoSource(ITextBuffer textBuffer)
        {
            return new QuickInfo(textBuffer);
        }
    }

    internal class QuickInfo : IAsyncQuickInfoSource
    {
        private readonly ITextBuffer buffer;
        private readonly ITagger<LunaScriptTag> tagger;

        public QuickInfo(ITextBuffer bufferIn)
        {
            buffer = bufferIn;
            tagger = new LunaScriptTagProvider().CreateTagger<LunaScriptTag>(buffer);
        }

        public void Dispose() {}

        private string getInfo(string token)
        {
            switch(token)
            {
                case "void":
                    return "the data type void tells the LunaScript complier that the function dose not expect return data";
                case "int8":
                    return "the data type int8 tells the LunaScript complier the data here is 8 bits wide and that it can have a negitive value";
                case "int16":
                    return "the data type int16 tells the LunaScript complier the data here is 16 bits wide and that it can have a negitive value";
                case "int32":
                    return "the data type int32 tells the LunaScript complier the data here is 32 bits wide and that it can have a negitive value";
                case "int64":
                    return "the data type int64 tells the LunaScript complier the data here is 64 bits wide and that it can have a negitive value";
                case "uint8":
                    return "the data type uint8 tells the LunaScript complier the data here is 8 bits wide and that it can not have a negitive value";
                case "uint16":
                    return "the data type uint16 tells the LunaScript complier the data here is 16 bits wide and that it can not have a negitive value";
                case "uint32":
                    return "the data type uint32 tells the LunaScript complier the data here is 32 bits wide and that it can not have a negitive value";
                case "uint64":
                    return "the data type uint64 tells the LunaScript complier the data here is 64 bits wide and that it can not have a negitive value";
                case "public":
                    return "public definer tells the LunaScript complier that this function can be accessed outside the current module";
                case "func":
                    return "func definer tells the LunaScript complier that this function can not be accessed outside the current module";
                case "module":
                    return "module definer tells the LunaScript complier the name of the module the code is for it is also used to stop dimond includes this would cause the compiler to get stuck when traversion module dependancys";
                case "import":
                    return "import definer tells the LunaScript complier that a module with name is required for this module to complie";
                case "->":
                    return "return definer tells the LunaScript complier that this function will return something other that void";
            }
            return "";
        }

        public async Task<QuickInfoItem> GetQuickInfoItemAsync(IAsyncQuickInfoSession session, CancellationToken cancellationToken)
        {
            SnapshotPoint? triggerPoint = session.GetTriggerPoint(buffer.CurrentSnapshot);
            if (triggerPoint != null)
            {
                ITextSnapshotLine line = triggerPoint.Value.GetContainingLine();
                ITrackingSpan lineSpan = buffer.CurrentSnapshot.CreateTrackingSpan(line.Extent, SpanTrackingMode.EdgeInclusive);

                SnapshotSpan span = line.Extent;
                ITagSpan<LunaScriptTag> tag = tagger.GetTags(new NormalizedSnapshotSpanCollection(new SnapshotSpan(span.Start, span.End)))
                                                                                    .FirstOrDefault(t => t.Span.Contains(triggerPoint.Value));
                var tagText = tag?.Tag?.token;
                if (tagText == null)
                    return await Task.FromResult<QuickInfoItem>(null);

                ClassifiedTextElement keywordElm = new ClassifiedTextElement(
                        new ClassifiedTextRun(PredefinedClassificationTypeNames.Text, "Keyword Doc: "),
                        new ClassifiedTextRun(PredefinedClassificationTypeNames.Text, getInfo(tagText)));
                List<object> content = new List<object> {keywordElm };
                return await Task.FromResult(new QuickInfoItem(lineSpan, new ContainerElement(ContainerElementStyle.Stacked, content.ToArray())));
            }

            return await Task.FromResult<QuickInfoItem>(null);
        }
    }
}
