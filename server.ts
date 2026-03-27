import { withHtmlLiveReload } from "bun-html-live-reload";

Bun.serve({
	port: 8080,
	fetch: withHtmlLiveReload(async (req) => {
		const html = await Bun.file("index.html").text();
		return new Response(html, {
			headers: { "Content-Type": "text/html" },
		});
	}),
});
console.log('listening on 8080')
