using Microsoft.Ajax.Utilities;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.Mvc;

namespace AdrenaWeb.Controllers
{
    public class UploadController : Controller
    {
        [HttpGet]
        public ActionResult AdrenaUpload() {
            return View();
        }

        [HttpPost]
        public ActionResult AdrenaUpload(HttpPostedFileBase Data, string MemberSecret, string AdrenaVersion) {
            try {
                if (ADRENA_GLOBALS.IsErasingOldFiles) {
                    ViewBag.LastError = "SYSTEM IS BEING CLEANED! TRY AGAIN IN A FEW SECONDS!";

                    return View();
                }

                string MbFolder = Server.MapPath("~/AdrenaMembers");
                ADRENA_MEMBER Mb = new ADRENA_MEMBER();
                if (!AdrenaMemberHelper.AdrenaMemberFindWithSecret(out Mb, MbFolder, MemberSecret)) {
                    ViewBag.LastError = "MEMBER DOES NOT EXIST!";

                    return View();
                }

                if(AdrenaMemberHelper.AdrenaMemberIsLicenseSingleUse(in Mb)) {
                    AdrenaMemberHelper.AdrenaMemberBan(MbFolder, Mb.Secret);

                    ViewBag.LastError = "YOUR LICENSE WAS SINGLE USE!";

                    return View();
                }

                if(AdrenaMemberHelper.AdrenaMemberHasLicenseExpired(in Mb)) {
                    AdrenaMemberHelper.AdrenaMemberBan(MbFolder, Mb.Secret);

                    ViewBag.LastError = "YOUR LICENSE HAS EXPIRED!";

                    return View();
                }

                ++Mb.NumberOfUploads;
                ViewBag.MemberName = Mb.Name;
                if (!AdrenaMemberHelper.AdrenaMemberUpdate(MbFolder, in Mb)) {
                    ViewBag.LastError = "CANNOT UPDATE MEMBER INFORMATION!";

                    return View();
                }

                string BinFolder = Server.MapPath("~/AdrenaBinaries");
                string InputFolder = Server.MapPath("~/AdrenaInput");
                string OutputFolder = Server.MapPath("~/AdrenaOutput");
                string ProtectedFile;
                string Identification;

                if (!AdrenaProtectHelper.AdrenaProtectFile(in Data, in AdrenaVersion, in BinFolder, in InputFolder,
                    in OutputFolder, out ProtectedFile, out Identification)) {
                    ViewBag.LastError = "AN INTERNAL ERROR OCCURED!";

                    return View();
                }

                ViewBag.ProtectLink = ProtectedFile;
                ViewBag.Identification = Identification;
                AdrenaSystem.AdrenaCleanIfNeeded(OutputFolder);

                return View();
            }

            catch {
                return View();
            }
        }
    }
}