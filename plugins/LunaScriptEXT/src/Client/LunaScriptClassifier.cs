namespace LunaScriptEXT.src.Client
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.Composition;
    using Microsoft.VisualStudio.Text;
    using Microsoft.VisualStudio.Text.Classification;
    using Microsoft.VisualStudio.Text.Tagging;
    using Microsoft.VisualStudio.Utilities;
    using LunaScript.Share;

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
        readonly IDictionary<Constants.WORD_TYPE, IClassificationType> LunaScriptTypesDictionary = new Dictionary<Constants.WORD_TYPE, IClassificationType>();
        internal LunaScriptClassifier(ITagAggregator<LunaScriptTag> tagAggregator, IClassificationTypeRegistryService typeService)
        {
            aggregator = tagAggregator;
            LunaScriptTypesDictionary[Constants.WORD_TYPE.VOID] = typeService.GetClassificationType("void");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.INT8] = typeService.GetClassificationType("int8");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.INT16] = typeService.GetClassificationType("int16");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.INT32] = typeService.GetClassificationType("int32");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.INT64] = typeService.GetClassificationType("int64");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.UINT8] = typeService.GetClassificationType("uint8");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.UINT16] = typeService.GetClassificationType("uint16");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.UINT32] = typeService.GetClassificationType("uint32");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.UINT64] = typeService.GetClassificationType("uint64");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.FLOAT32] = typeService.GetClassificationType("float32");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.FLOAT64] = typeService.GetClassificationType("float64");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.FUNC] = typeService.GetClassificationType("func");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.PUBLIC] = typeService.GetClassificationType("public");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.RET] = typeService.GetClassificationType("ret");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.AND] = typeService.GetClassificationType("and");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.OR] = typeService.GetClassificationType("or");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.MOD] = typeService.GetClassificationType("mod");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.XOR] = typeService.GetClassificationType("xor");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.GLOBAL] = typeService.GetClassificationType("global");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.IMPORT] = typeService.GetClassificationType("import");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.MODULE] = typeService.GetClassificationType("module");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.SUPER] = typeService.GetClassificationType("->");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.L_SQUIGGLY] = typeService.GetClassificationType("{");
            LunaScriptTypesDictionary[Constants.WORD_TYPE.R_SQUIGGLY] = typeService.GetClassificationType("}");
        }

        public event EventHandler<SnapshotSpanEventArgs> TagsChanged
        {
            add { }
            remove { }
        }

        public IEnumerable<ITagSpan<ClassificationTag>> GetTags(NormalizedSnapshotSpanCollection spans)
        {
            foreach (var tagSpan in aggregator.GetTags(spans))
            {
                var tagSpans = tagSpan.Span.GetSpans(spans[0].Snapshot);
                yield return new TagSpan<ClassificationTag>(tagSpans[0], new ClassificationTag(LunaScriptTypesDictionary[tagSpan.Tag.type]));
            }
        }
    }
}