using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Windows.Media;
using Microsoft.VisualStudio.Text;
using Microsoft.VisualStudio.Text.Classification;
using Microsoft.VisualStudio.Text.Tagging;
using Microsoft.VisualStudio.Utilities;

namespace LunaScriptEXT.src.Client
{
    internal static class SymbolObjectType
    {
        [Export(typeof(ClassificationTypeDefinition))]
        [Name("symbol")]
        internal static ClassificationTypeDefinition symbolExclamation = null;

        [Export(typeof(EditorFormatDefinition))]
        [ClassificationType(ClassificationTypeNames = "symbol")]
        [Name("symbol")]
        [UserVisible(false)]
        [Order(Before = Priority.Default)]
        internal sealed class SymbolType : ClassificationFormatDefinition
        {
            public SymbolType()
            {
                DisplayName = "symbol";
                ForegroundColor = Colors.LightGoldenrodYellow;
            }
        }
    }

    [Export(typeof(ITaggerProvider))]
    [ContentType("LunaScript")]
    [TagType(typeof(ClassificationTag))]
    internal sealed class LunaScriptClassifierProvider : ITaggerProvider
    {

        [Export]
        [Name("LunaScript")]
        [BaseDefinition("code")]
        internal static ContentTypeDefinition LunaScriptContentType = null;

        [Export]
        [FileExtension(".lls")]
        [ContentType("LunaScript")]
        internal static FileExtensionToContentTypeDefinition LunaScriptFileType = null;

        [Import]
        internal IClassificationTypeRegistryService ClassificationTypeRegistry = null;

        [Import]
        internal IBufferTagAggregatorFactoryService aggregatorFactory = null;

        public ITagger<T> CreateTagger<T>(ITextBuffer buffer) where T : ITag
        {
            ITagAggregator<LunaScriptTag> tagAggregator = aggregatorFactory.CreateTagAggregator<LunaScriptTag>(buffer);
            return new LunaScriptClassifier(tagAggregator, ClassificationTypeRegistry) as ITagger<T>;
        }
    }

    internal sealed class LunaScriptClassifier : ITagger<ClassificationTag>
    {
        readonly ITagAggregator<LunaScriptTag> aggregator;
        readonly IClassificationType keyword_type;
        readonly IClassificationType symbol_type;
        internal LunaScriptClassifier(ITagAggregator<LunaScriptTag> tagAggregator, IClassificationTypeRegistryService typeService)
        {
            aggregator = tagAggregator;
            keyword_type = typeService.GetClassificationType("keyword");
            symbol_type = typeService.GetClassificationType("symbol");
        }

        public event EventHandler<SnapshotSpanEventArgs> TagsChanged
        {
            add { }
            remove { }
        }

        public IEnumerable<ITagSpan<ClassificationTag>> GetTags(NormalizedSnapshotSpanCollection spans)
        {
            foreach (IMappingTagSpan<LunaScriptTag> tagSpan in aggregator.GetTags(spans))
                yield return new TagSpan<ClassificationTag>(tagSpan.Span.GetSpans(spans[0].Snapshot)[0], new ClassificationTag(tagSpan.Tag.isKeyword ? keyword_type : symbol_type));
            yield break;
        }
    }
}