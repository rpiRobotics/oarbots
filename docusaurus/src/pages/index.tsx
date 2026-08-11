import Layout from "@theme/Layout";
import Link from "@docusaurus/Link";
import useBaseUrl from "@docusaurus/useBaseUrl";

export default function Home() {
    return (
        <Layout>
            <main>
                <div className="flex flex-col justify-center items-center w-screen py-24 bg-gray-100 dark:bg-gray-800">
                    <h1 className="text-5xl">OARBots Documentation</h1>
                    <span className="h-2"></span>
                    <div className="flex flex-row justify-center items-center gap-4">
                        <Link className="hover:no-underline text-2xl bg-blue-300 dark:bg-blue-900 hover:bg-blue-400 hover:dark:bg-blue-800 px-6 py-2 text-black dark:text-white rounded-xl font-semibold transition hover:-translate-y-0.5 hover:drop-shadow-md" to="/docs/getting-started/powering-up-the-oarbots">
                            View the Docs
                        </Link>
                        <Link className="hover:no-underline text-2xl bg-gray-800 dark:bg-gray-950 hover:bg-gray-900 hover:dark:bg-gray-900 px-6 py-2 text-white rounded-xl font-semibold transition hover:-translate-y-0.5 hover:drop-shadow-md flex flex-row justify-center items-center gap-2" to="https://github.com/rpiRobotics/oarbots/">
                            <img className="size-6" src={useBaseUrl("/img/GitHub_Invertocat_White.svg")} alt="GitHub Logo" />
                            View on GitHub
                        </Link>
                    </div>
                </div>
                <div className="flex flex-col py-6 px-12">
                    <p className="font-medium text-md ">
                        Rensselaer Polytechnic Institute's Center for Smart Convergent Manufacturing Systems (CSCMS)
                        developed and built two omni-directional assistive robots, or OARBots, for use in various
                        research projects and industrial applications. The OARBot system runs ROS2 and consists of
                        sensors and external computers in addition to the OARBots themselves. This site serves
                        to:
                    </p>
                    <ul>
                        <li>
                            Document their physical makeup, including parts, electronics, and sensors
                        </li>
                        <li>
                            Provide usage instructions to others
                        </li>
                        <li>
                            Document common troubleshooting steps
                        </li>
                        <li>
                            Explain the inner-workings of the OARBot code for future maintainers
                        </li>
                    </ul>
                    <p className="font-medium text-md ">
                        If you find errors or would like to add to the documentation, submit a pull request to
                        the <a href="https://github.com/rpiRobotics/oarbots">OARBots GitHub</a>. Under each doc, there
                        is an "edit this page" link which will take you right to the file that generated the doc. Our
                        docs are written in <a href="https://mdxjs.com/">MDX</a>, which supports
                        standard <a href="https://www.markdownguide.org/">Markdown</a> syntax with additional HTML/JS
                        support.
                    </p>
                </div>
            </main>
        </Layout>
    );
}