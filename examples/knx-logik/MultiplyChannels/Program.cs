using System;
using System.Collections.Generic;
using System.Xml;
using System.IO;

namespace MultiplyChannels {
    class Program {
        const int cChannelCount = 50;
        const int cParameterBlockOffset = 12;
        const int cParameterBlockSize = 110;

        static XmlNamespaceManager nsmgr;

        static Dictionary<string, string> gPath = new Dictionary<string, string>() {
            {"/knx:KNX/knx:Parameters/knx:Parameter", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:Parameters" },
            {"/knx:KNX/knx:ParameterRefs/knx:ParameterRef", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:ParameterRefs" },
            {"/knx:KNX/knx:ComObjectTable/knx:ComObject", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:ComObjectTable" },
            {"/knx:KNX/knx:ComObjectRefs/knx:ComObjectRef", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:ComObjectRefs" },
            {"/knx:KNX/knx:Dynamic/knx:Channel", "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Dynamic" },
        };

        static List<string> gSizePath = new List<string>() {
            "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:Code/knx:RelativeSegment",
            "/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:ApplicationPrograms/knx:ApplicationProgram/knx:Static/knx:Code/knx:LoadProcedures/knx:LoadProcedure/knx:LdCtrlRelSegment",
        };

        static void ProcessAttribute(int iChannel, XmlNode iTargetNode) {
            foreach (XmlAttribute lAttr in iTargetNode.Attributes) {
                lAttr.Value = lAttr.Value.Replace("%C%", iChannel.ToString());
                lAttr.Value = lAttr.Value.Replace("%C1%", ((iChannel - 1) * 3 + 1).ToString());
                lAttr.Value = lAttr.Value.Replace("%C2%", ((iChannel - 1) * 3 + 2).ToString());
                lAttr.Value = lAttr.Value.Replace("%C3%", ((iChannel - 1) * 3 + 3).ToString());
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

        static void ProcessFinish(XmlNode iTargetNode) {
            // set the right Size attributes
            XmlNodeList lNodes = iTargetNode.SelectNodes("//*[@Size='%S%']");
            foreach (XmlNode lNode in lNodes) {
                lNode.Attributes.GetNamedItem("Size").Value = (cChannelCount * cParameterBlockSize + cParameterBlockOffset).ToString();
            }
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

            XmlNodeList lAttrs = iTargetNode.SelectNodes("//*/@*[string-length() > '13']");
            foreach (XmlNode lAttr in lAttrs) {
                if (lAttr.Value != null) lAttr.Value = lAttr.Value.Replace(lOldId, lNewId);
            }
            // create registration entry
            XmlNode lHardwareVersionAttribute = iTargetNode.SelectSingleNode("/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:Hardware/knx:Hardware/@VersionNumber", nsmgr);
            int lHardwareVersion = int.Parse(lHardwareVersionAttribute.Value);
            XmlNode lRegistrationNumber = iTargetNode.SelectSingleNode("/knx:KNX/knx:ManufacturerData/knx:Manufacturer/knx:Hardware/knx:Hardware/knx:Hardware2Programs/knx:Hardware2Program/knx:RegistrationInfo/@RegistrationNumber", nsmgr);
            lRegistrationNumber.Value = string.Format("0001/{0}{1}", lHardwareVersion, lApplicationVersion);

            // // Add ReplacesVersions 
            // if (lReplacesVersionsAttribute != null) {
            //     string lReplacesVersions = lReplacesVersionsAttribute.Value;
            //     string lOldVerion = string.Format(" {0}", lApplicationVersion - 1);
            //     if (!lReplacesVersions.Contains(lOldVerion) && lReplacesVersions != (lApplicationVersion - 1).ToString()) lReplacesVersionsAttribute.Value += lOldVerion;
            // }
        }

        static string CalculateId(int iApplicationNumber, int iApplicationVersion) {
            return string.Format("-{0:X4}-{1:X2}-0000", iApplicationNumber, iApplicationVersion);

        }
        static void Main(string[] args) {

            XmlDocument lResult = new XmlDocument();
            lResult.Load(Path.Combine("..", "Logikmodul.frame.xml"));
            XmlDocument lTemplate = new XmlDocument();
            lTemplate.Load(Path.Combine("..", "Logikmodul.templ.xml"));

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
                        if (lTargetNode.Name == "Parameter")
                            ProcessParameter(lChannel, lTargetNode);
                        if (lTargetNode.Name == "Channel")
                            ProcessChannel(lChannel, lTargetNode);
                    } // foreach <Parameter> node
                }
            }
            ProcessFinish(lResult);
            lResult.Save(Path.Combine("..", "Logikmodul.test.xml"));
        }
    }
}
