using System;
using System.Collections.Generic;
using System.Xml;
using System.IO;
using System.Text.RegularExpressions;
using System.Reflection;
using System.Configuration;
using System.Text;

//using Knx.Ets.Converter.ConverterEngine;

namespace MultiplyChannels {
    class Program {
        // number of logic channels which will be implemented in this logik module
        static int cChannelCount = 50;
        // parameter size (bytes) used by parameters not used by any logik channel
        static int cParameterBlockOffset = 12;
        // parameter size (bytes) used by parameters per channel
        static int cParameterBlockSize = 110;
        // KoOffset represents the first KO of the logic module. Ensure, that
        // (cChannelCount * 3) + cKoOffset - 1 is less than 250 !!
        // i.e. 50 Channels: 50 * 3 + 100 - 1 = 249 < 250
        static int cKoOffset = 100; // the lowest allowed value is 1!!!

        static XmlNamespaceManager nsmgr;

        private const string _toolName = "KNX MT";
        private const string _toolVersion = "5.1.255.16695";

        static int GetHeaderParameter(string iHeaderFileContent, string iDefineName) {
            string lPattern = "#define.*" + iDefineName + @"\s*(\d{1,4})";
            Match m = Regex.Match(iHeaderFileContent, lPattern, RegexOptions.None);
            int lResult = 0;
            if (m.Groups.Count > 1) {
                int.TryParse(m.Groups[1].Value, out lResult);
            }
            return lResult;
        }

        static bool ParseHeaderFile(string iHeaderFileContent) {
            cChannelCount = GetHeaderParameter(iHeaderFileContent, "NUM_Channels");
            cParameterBlockOffset = GetHeaderParameter(iHeaderFileContent, "NUM_paramOffset");
            cParameterBlockSize = GetHeaderParameter(iHeaderFileContent, "NUM_paramBlockSize");
            cKoOffset = GetHeaderParameter(iHeaderFileContent, "KO_Offset");
            return (cChannelCount > 0) && (cParameterBlockOffset > 0) && (cParameterBlockSize > 0) && (cKoOffset > 0);
        }

        static Dictionary<string, string> gPath = new Dictionary<string, string>() {
            {"/knx:KNX/knx:Parameters/knx:Parameter", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:Parameters" },
            {"/knx:KNX/knx:ParameterRefs/knx:ParameterRef", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:ParameterRefs" },
            {"/knx:KNX/knx:ComObjectTable/knx:ComObject", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:ComObjectTable" },
            {"/knx:KNX/knx:ComObjectRefs/knx:ComObjectRef", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:ComObjectRefs" },
            {"/knx:KNX/knx:Dynamic/*[1]", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Dynamic" },
        };

        static List<string> gSizePath = new List<string>() {
            "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:Code/knx:RelativeSegment",
            "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:Code/knx:LoadProcedures/knx:LoadProcedure/knx:LdCtrlRelSegment",
        };

        static void ProcessAttribute(int iChannel, XmlNode iTargetNode) {
            foreach (XmlAttribute lAttr in iTargetNode.Attributes) {
                lAttr.Value = lAttr.Value.Replace("%C%", iChannel.ToString());
                lAttr.Value = lAttr.Value.Replace("%C1%", ((iChannel - 1) * 3 + cKoOffset).ToString());
                lAttr.Value = lAttr.Value.Replace("%C2%", ((iChannel - 1) * 3 + cKoOffset + 1).ToString());
                lAttr.Value = lAttr.Value.Replace("%C3%", ((iChannel - 1) * 3 + cKoOffset + 2).ToString());
            }

        }
        static void ProcessParameter(int iChannel, XmlNode iTargetNode) {
            //calculate new offset
            XmlNode lMemory = iTargetNode.SelectSingleNode("knx:Memory", nsmgr);
            if (lMemory != null) {
                XmlNode lAttr = lMemory.Attributes.GetNamedItem("Offset");
                int lOffset = int.Parse(lAttr.Value);
                lOffset += cParameterBlockOffset + (iChannel - 1) * cParameterBlockSize;
                lAttr.Value = lOffset.ToString();
            }
        }

        static void ProcessChannel(int iChannel, XmlNode iTargetNode) {
            //attributes of the node
            if (iTargetNode.Attributes != null) {
                ProcessAttribute(iChannel, iTargetNode);
            }

            //Print individual children of the node, gets only direct children of the node
            XmlNodeList lChildren = iTargetNode.ChildNodes;
            foreach (XmlNode lChild in lChildren) {
                ProcessChannel(iChannel, lChild);
            }
        }

        static bool ProcessFinish(XmlNode iTargetNode) {
            // set the right Size attributes
            XmlNodeList lNodes = iTargetNode.SelectNodes("//*[@Size='%S%']");
            string lSize = (cChannelCount * cParameterBlockSize + cParameterBlockOffset).ToString();
            foreach (XmlNode lNode in lNodes) {
                lNode.Attributes.GetNamedItem("Size").Value = lSize;
            }
            Console.WriteLine("- Final parameter size is {0}", lSize);
            // set number of Channels
            lNodes = iTargetNode.SelectNodes("//*[@Value='%N%']");
            foreach (XmlNode lNode in lNodes) {
                lNode.Attributes.GetNamedItem("Value").Value = cChannelCount.ToString();
            }
            lNodes = iTargetNode.SelectNodes("//*[@maxInclusive='%N%']");
            foreach (XmlNode lNode in lNodes) {
                lNode.Attributes.GetNamedItem("maxInclusive").Value = cChannelCount.ToString();
            }
            // change all Id-Attributes
            XmlNode lApplicationProgramNode = iTargetNode.SelectSingleNode("/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram", nsmgr);
            int lApplicationNumber = int.Parse(lApplicationProgramNode.Attributes.GetNamedItem("ApplicationNumber").Value);
            int lApplicationVersion = int.Parse(lApplicationProgramNode.Attributes.GetNamedItem("ApplicationVersion").Value);
            XmlNode lReplacesVersionsAttribute = lApplicationProgramNode.Attributes.GetNamedItem("ReplacesVersions");
            string lOldId = CalculateId(1, 1);
            string lNewId = CalculateId(lApplicationNumber, lApplicationVersion);
            int lParameterSeparatorCount = 1;
            XmlNodeList lAttrs = iTargetNode.SelectNodes("//*/@*[string-length() > '13']");
            foreach (XmlNode lAttr in lAttrs) {
                if (lAttr.Value != null) {
                    lAttr.Value = lAttr.Value.Replace(lOldId, lNewId);
                    // ParameterSelector is renumbered
                    if (lAttr.Value.Contains("_PS-")) {
                        lAttr.Value = string.Format("{0}-{1}", lAttr.Value.Substring(0, lAttr.Value.LastIndexOf('-')), lParameterSeparatorCount);
                        lParameterSeparatorCount += 1;
                    }
                }
            }
            Console.WriteLine("- ApplicationNumber: {0}, ApplicationVersion: {1}, calculated ID is: {2}", lApplicationNumber, lApplicationVersion, lNewId);

            // create registration entry
            XmlNode lHardwareVersionAttribute = iTargetNode.SelectSingleNode("/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:Hardware/knx:Hardware/@VersionNumber", nsmgr);
            int lHardwareVersion = int.Parse(lHardwareVersionAttribute.Value);
            XmlNode lRegistrationNumber = iTargetNode.SelectSingleNode("/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:Hardware/knx:Hardware/knx:Hardware2Programs/knx:Hardware2Program/knx:RegistrationInfo/@RegistrationNumber", nsmgr);
            lRegistrationNumber.Value = string.Format("0001/{0}{1}", lHardwareVersion, lApplicationVersion);
            Console.WriteLine("- RegistrationVersion is: {0}", lRegistrationNumber.Value);

            // Add ReplacesVersions 
            if (lReplacesVersionsAttribute != null) {
                string lReplacesVersions = lReplacesVersionsAttribute.Value;
                Console.WriteLine("- ReplacesVersions entry is: {0}", lReplacesVersions);
                // string lOldVerion = string.Format(" {0}", lApplicationVersion - 1);
                // if (!lReplacesVersions.Contains(lOldVerion) && lReplacesVersions != (lApplicationVersion - 1).ToString()) lReplacesVersionsAttribute.Value += lOldVerion;
            }
            Console.WriteLine("Sanity checks... ");
            Console.Write("- Id-Uniqueness...");
            bool lFailUnique = false;
            lNodes = iTargetNode.SelectNodes("//*[@Id]");
            Dictionary<string, bool> lIds = new Dictionary<string, bool>();
            foreach (XmlNode lNode in lNodes) {
                if (lNode.Name != "Enumeration") {
                    string lId = lNode.Attributes.GetNamedItem("Id").Value;
                    if (lIds.ContainsKey(lId)) {
                        if (!lFailUnique) Console.WriteLine();
                        Console.WriteLine("  {0} is a duplicate Id", lId);
                        lFailUnique = true;
                    } else {
                        lIds.Add(lId, true);
                    }
                }
            }
            if (!lFailUnique) Console.WriteLine("finished");

            Console.Write("- IdRef-Integrity...");
            bool lFailRef = false;
            lNodes = iTargetNode.SelectNodes("//*[@RefId]");
            foreach (XmlNode lNode in lNodes) {
                if (lNode.Name != "Manufacturer") {
                    string lRefId = lNode.Attributes.GetNamedItem("RefId").Value;
                    if (!lIds.ContainsKey(lRefId)) {
                        if (!lFailRef) Console.WriteLine();
                        Console.WriteLine("  {0} is referenced in {1}, but not defined", lRefId, lNode.Name);
                        lFailRef = true;
                    }
                }
            }
            if (!lFailRef) Console.WriteLine("finished");

            return !(lFailUnique || lFailRef);
        }

        static string CalculateId(int iApplicationNumber, int iApplicationVersion) {
            return string.Format("-{0:X4}-{1:X2}-0000", iApplicationNumber, iApplicationVersion);
        }

        #region Reflection
        private static object InvokeMethod(Type type, string methodName, object[] args) {

            var mi = type.GetMethod(methodName, BindingFlags.Static | BindingFlags.NonPublic);
            return mi.Invoke(null, args);
        }

        private static void SetProperty(Type type, string propertyName, object value) {
            PropertyInfo prop = type.GetProperty(propertyName, BindingFlags.NonPublic | BindingFlags.Static);
            prop.SetValue(null, value, null);
        }
        #endregion

        private static void ExportKnxprod(string iXmlFileName, string iKnxprodFileName) {
            try {
                var files = new string[] { iXmlFileName };
                var asmPath = Path.Combine(@"C:\Program Files (x86)\ETS4", "Knx.Ets.Converter.ConverterEngine.dll");
                var asm = Assembly.LoadFrom(asmPath);
                var eng = asm.GetType("Knx.Ets.Converter.ConverterEngine.ConverterEngine");
                var bas = asm.GetType("Knx.Ets.Converter.ConverterEngine.ConvertBase");

                Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
                InvokeMethod(bas, "Uninitialize", null);
                //ConvertBase.Uninitialize();
                //var dset = ConverterEngine.BuildUpRawDocumentSet( files );
                var dset = InvokeMethod(eng, "BuildUpRawDocumentSet", new object[] { files });
                //ConverterEngine.CheckOutputFileName(outputFile, ".knxprod");
                InvokeMethod(eng, "CheckOutputFileName", new object[] { iKnxprodFileName, ".knxprod" });
                //ConvertBase.CleanUnregistered = false;
                //SetProperty(bas, "CleanUnregistered", false);
                //dset = ConverterEngine.ReOrganizeDocumentSet(dset);
                dset = InvokeMethod(eng, "ReOrganizeDocumentSet", new object[] { dset });
                //ConverterEngine.PersistDocumentSetAsXmlOutput(dset, outputFile, null, string.Empty, true, _toolName, _toolVersion);
                InvokeMethod(eng, "PersistDocumentSetAsXmlOutput", new object[] { dset, iKnxprodFileName, null, "", true, _toolName, _toolVersion });
                Console.WriteLine("Output of {0} successful", iKnxprodFileName);
            }
            catch (Exception ex) {
                Console.WriteLine("Error during knxprod creation:");
                Console.WriteLine(ex.ToString());
            }
        }

        static void Main(string[] args) {

            if (args.Length == 0) {
                Console.WriteLine("Usage: MultiplyChannels <PathToProject>");
                Console.WriteLine("       MultiplyChannels --just-knxprod <PathToProject>");
                return;
            }
            string lProject = args[0];
            bool lJustKnxprod = false;
            bool lSuccess = false;
            if (lProject.ToLower() == "--just-knxprod") {
                lJustKnxprod = true;
                lProject = args[1];
            }
            string lHeaderFileName = Path.ChangeExtension(lProject, "h");
            string lOutputFileName = Path.ChangeExtension(lProject, "out.xml");
            string lKnxprodFileName = Path.ChangeExtension(lProject, "knxprod");
            string lHeaderFile = File.OpenText(lHeaderFileName).ReadToEnd();
            bool lHeader = ParseHeaderFile(lHeaderFile);
            if (!lJustKnxprod && lHeader) {
                Console.WriteLine("Processing {0} with:", lProject);
                Console.WriteLine("- NUM_Channels = {0}", cChannelCount);
                Console.WriteLine("- NUM_paramOffset = {0}", cParameterBlockOffset);
                Console.WriteLine("- NUM_paramBlockSize = {0}", cParameterBlockSize);
                Console.WriteLine("- KO_Offset = {0}", cKoOffset);
                XmlDocument lResult = new XmlDocument();
                lResult.Load(Path.ChangeExtension(lProject, "frame.xml"));
                XmlDocument lTemplate = new XmlDocument();
                lTemplate.Load(Path.ChangeExtension(lProject, "templ.xml"));

                nsmgr = new XmlNamespaceManager(lResult.NameTable);
                nsmgr.AddNamespace("knx", "http://knx.org/xml/project/11");

                for (int lChannel = 1; lChannel <= cChannelCount; lChannel++) {
                    foreach (var lPath in gPath) {
                        XmlNodeList lSourceNodes = lTemplate.SelectNodes(lPath.Key, nsmgr); // get all <Parameter> nodes
                        XmlNode lTargetNodes = lResult.SelectSingleNode(lPath.Value, nsmgr); // get all <Parameter> nodes
                        foreach (XmlNode lSourceNode in lSourceNodes) // for each <Parameter> node
                        {
                            XmlNode lTargetNode = lResult.ImportNode(lSourceNode, true);
                            lTargetNodes.AppendChild(lTargetNode);
                            ProcessAttribute(lChannel, lTargetNode);
                            if (lTargetNode.Name == "Parameter") {
                                ProcessParameter(lChannel, lTargetNode);
                            } else if (lPath.Value.EndsWith("Dynamic")) {
                                if (lTargetNode.Name == "Channel") {
                                    ProcessChannel(lChannel, lTargetNode);
                                } else {
                                    XmlNode lChannelNode = lTargetNode.SelectSingleNode(".//knx:Channel[1]", nsmgr);
                                    if (lChannelNode != null && lChannelNode.Name == "Channel")
                                        ProcessChannel(lChannel, lChannelNode);
                                }
                            }
                        } // foreach <Parameter> node
                    }
                }
                lSuccess = ProcessFinish(lResult);
                lResult.Save(lOutputFileName);
                Console.WriteLine("Output written to {0}", lOutputFileName);
            }
            if (lSuccess || lJustKnxprod) {
                ExportKnxprod(lOutputFileName, lKnxprodFileName);
                lHeader = true;
            } else {
                Console.WriteLine("--> knxprod not created due to failed sanity checks <--");
            }
            if (!lHeader) {
                Console.WriteLine("{0} has no definition for", lHeaderFileName);
                if (cChannelCount == 0) Console.WriteLine("- NUM_Channels");
                if (cParameterBlockOffset == 0) Console.WriteLine("- NUM_paramOffset");
                if (cParameterBlockSize == 0) Console.WriteLine("- NUM_paramBlockSize");
                if (cKoOffset == 0) Console.WriteLine("- KO_Offset");
            }
        }
    }
}
