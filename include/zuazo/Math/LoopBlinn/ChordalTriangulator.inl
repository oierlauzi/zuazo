#include "ChordalTriangulator.h"

namespace Zuazo::Math::LoopBlinn {

template<typename T, typename I>
constexpr ChordalTriangulator<T, I>::VertexReference::VertexReference(	index_type front,
																		index_type back,
																		index_type helper ) noexcept
	: front(front)
	, back(back)
	, helper(helper)
{
}

template<typename T, typename I>
inline ChordalTriangulator<T, I>::ChordalTriangulator(	std::vector<index_type>& indices,
														std::vector<vertex_type>& chordalAxis,
														Utils::BufferView<const vertex_type> vertices,
														Utils::BufferView<const VertexReference> references,
														index_type chordalBaseIndex,
														index_type restartIndex )
	: m_indices(indices)
	, m_chordalAxis(chordalAxis)
	, m_vertices(vertices)
	, m_references(references)
	, m_chordalBaseIndex(chordalBaseIndex)
	, m_restartIndex(restartIndex)
{
}

template<typename T, typename I>
inline TriangleSideFlags ChordalTriangulator<T, I>::operator()(	const typename triangulator_type::Diagonal& diagonal, 
																typename triangulator_type::index_type bestVertex,
																TriangleSideFlags adjointDiagonals,
																size_t queueLength,
																bool falseDiag )
{
	//Shorthand for variables
	auto& indices = m_indices.get();
	auto& chordalAxis = m_chordalAxis.get();
	const auto& vertices = m_vertices;
	const auto& references = m_references;
	const auto chordalBaseIndex = m_chordalBaseIndex;
	const auto restartIndex = m_restartIndex;

	//Obtain the positions of potential vertices
	const auto& diagonalFirstPos = vertices[references[diagonal.first].front].pos;
	const auto& diagonalSecondPos = vertices[references[diagonal.second].front].pos;
	const auto& bestVertexPos = vertices[references[bestVertex].front].pos;
	const auto chordalPos = (diagonalFirstPos + diagonalSecondPos) / 2;
	const auto nextChordal0Pos = (diagonalFirstPos + bestVertexPos) / 2;
	const auto nextChordal1Pos = (diagonalSecondPos + bestVertexPos) / 2;
	const auto centroid = (diagonalFirstPos + diagonalSecondPos + bestVertexPos) / 3;

	//Obtain the previous indices
	const auto preBestVertex = (bestVertex > 0 ? bestVertex : references.size()) - 1;
	const auto preDiagonal = typename triangulator_type::Diagonal(
		(diagonal.first > 0 ? diagonal.first : references.size()) - 1,
		(diagonal.second > 0 ? diagonal.second : references.size()) - 1
	);

	//Obtain the preferred vertex


	//Restart a new primitive
	if(!indices.empty()) {
		indices.push_back(restartIndex);
	}

	//Based on the number of adjacent diagonals, triangulate this triangle
	if(falseDiag) {
		//Treat specially the first tri, as its diagonal is actually an edge
		switch (adjointDiagonals) {
		case TriangleSideFlags::NONE: {
			//This triangle has no adjacent diagonals
			//There is only one possible ordering
			//Draw 4 triangles:
			//
			//                X bestVertex
			//               /|\
			//              / | \
			//			   /  |  \
			//            /  _X_  \ <--- centroid
			//           /__/   \__\
			//   second X-----------X first <-current diagonal
			//
			//
			assert(preBestVertex == diagonal.first);
			assert(preDiagonal.second == bestVertex);
			assert(preDiagonal.first == diagonal.second);

			//Create the three triangles and add them
			const std::array<index_type, 11> triangles = {
				references[diagonal.first].front,
				references[bestVertex].back,
				chordalBaseIndex + chordalAxis.size() + 0,	//centroid
				restartIndex, //--------------------------
				references[bestVertex].front,
				references[diagonal.second].back,
				chordalBaseIndex + chordalAxis.size() + 1,	//centroid
				restartIndex, //--------------------------
				references[diagonal.second].front,
				references[diagonal.first].back,
				chordalBaseIndex + chordalAxis.size() + 2,	//centroid
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 3> chordalVertices = {
				getChordalVertex(diagonal.first, centroid),			//0

				getChordalVertex(bestVertex, centroid),				//1

				getChordalVertex(diagonal.second, centroid),		//2		
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		case TriangleSideFlags::LEFT: {
			//This diagonal has another adjacent diagonal
			//Draw 2 triangles:
			//
			//                X bestVertex
			//               / \
			//              /   \
			//nextChordal1 X     \
			//            / \__   \
			//           /     \__ \
			//   second X-----------X first <-current diagonal
			//
			//
			assert(preBestVertex == diagonal.first);
			assert(preDiagonal.first == diagonal.second);

			//Create the three triangles and add them
			const std::array<index_type, 7> triangles = {
				references[diagonal.second].front,
				references[diagonal.first].back,
				chordalBaseIndex + chordalAxis.size() + 0, //nextChordal1
				restartIndex, //--------------------------
				references[diagonal.first].front,
				references[bestVertex].back,
				chordalBaseIndex + chordalAxis.size() + 1  //nextChordal1
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 2> chordalVertices = {
				getChordalVertex(diagonal.second, nextChordal1Pos),	//0

				getChordalVertex(diagonal.first, nextChordal1Pos)	//1
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		case TriangleSideFlags::RIGHT: {
			//This diagonal has another adjacent diagonal
			//Draw 2 triangles:
			//
			//             X bestVertex
			//            / \
			//           /   \
			//          /     X nextChordal0
			//         /   __/ \
			//        / __/     \
			//second X-----------X first <-current diagonal
			//         chordal
			//
			assert(preDiagonal.second == bestVertex);
			assert(preDiagonal.first == diagonal.second);

			//Create the three triangles and add them
			const std::array<index_type, 7> triangles = {
				references[bestVertex].front,
				references[diagonal.second].back,
				chordalBaseIndex + chordalAxis.size() + 0, //nextChordal0
				restartIndex, //--------------------------
				references[diagonal.second].front,
				references[diagonal.first].back,
				chordalBaseIndex + chordalAxis.size() + 1  //nextChordal0
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 2> chordalVertices = {
				getChordalVertex(bestVertex, nextChordal0Pos),		//0

				getChordalVertex(diagonal.second, nextChordal0Pos)	//1
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		case TriangleSideFlags::BOTH: {
			//This triangle has another 2 adjacent diagonals.
			//There is only one possible ordering
			//Draw 3 triangles:
			//
			//                X bestVertex
			//               / \
			//              /   \
			//nextChordal1 X-----X nextChordal0
			//            /  __/  \
			//           /__/      \
			//   second X-----------X first <-current diagonal
			//             chordal
			//
			assert(preDiagonal.first == diagonal.second);

			//Create the three triangles and add them
			const std::array<index_type, 8> triangles = {
				references[diagonal.first].back,
				chordalBaseIndex + chordalAxis.size() + 0, //nextChordal0
				references[diagonal.second].front,
				chordalBaseIndex + chordalAxis.size() + 1, //nextChordal1
				restartIndex, //--------------------------
				chordalBaseIndex + chordalAxis.size() + 2, //nextChordal0
				references[bestVertex].front,
				chordalBaseIndex + chordalAxis.size() + 3, //nextChordal1
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 4> chordalVertices = {
				getChordalVertex(diagonal.second, nextChordal0Pos),	//0
				getChordalVertex(diagonal.second, nextChordal1Pos),	//1

				getChordalVertex(bestVertex, nextChordal0Pos),		//2
				getChordalVertex(bestVertex, nextChordal1Pos)		//3
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		default: {
			assert(!"No way!, we got here"); //Remember that we've optimized away lonely triangle inputs
			//In a release version, at least, draw a tri ignoring the
			//chordal axis
			const std::array<index_type, 3> triangle = {
				chordalBaseIndex + chordalAxis.size() + 0,
				chordalBaseIndex + chordalAxis.size() + 1,
				chordalBaseIndex + chordalAxis.size() + 2
			};
			indices.insert(indices.cend(), triangle.cbegin(), triangle.cend());

			const std::array<vertex_type, 3> chordalVertices = {
				vertex_type(diagonalFirstPos, 	klm_vector_type(-1)),
				vertex_type(bestVertexPos,		klm_vector_type(-1)),
				vertex_type(diagonalSecondPos,	klm_vector_type(-1))
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		}

	} else {
		switch (adjointDiagonals) {
		case TriangleSideFlags::NONE: {
			//This diagonal is the last one for this chordal line.
			//Draw 2 triangles:
			//
			//            X bestVertex
			//           /|\
			//          / | \
			//         /  |  \
			//        /   |   \
			//second X----X----X first <-current diagonal
			//         chordal
			//
			assert(preBestVertex == diagonal.first);
			assert(preDiagonal.second == bestVertex);

			//Create both triangles and add them
			const std::array<index_type, 7> triangles = {
				references[diagonal.first].front,
				references[bestVertex].back,
				chordalBaseIndex + chordalAxis.size() + 0, //chordal
				restartIndex, //--------------------------
				chordalBaseIndex + chordalAxis.size() + 1, //chordal
				references[bestVertex].front,
				references[diagonal.second].back
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 2> chordalVertices = {
				getChordalVertex(diagonal.first, chordalPos),	//0

				getChordalVertex(bestVertex, chordalPos)		//1
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		case TriangleSideFlags::LEFT: {
			//This diagonal has another adjacent diagonal
			//Draw 3 triangles:
			//
			//                X bestVertex
			//               /|\
			//              / | \
			//nextChordal1 X  |  \
			//            / \ |   \
			//           /   \|    \
			//   second X-----X-----X first <-current diagonal
			//             chordal
			//
			assert(preBestVertex == diagonal.first); //They share a line

			//Create the three triangles and add them
			const std::array<index_type, 8> triangles = {
				references[diagonal.first].front,
				references[bestVertex].back,
				chordalBaseIndex + chordalAxis.size() + 0, //chordal
				chordalBaseIndex + chordalAxis.size() + 1, //nextChordal1
				restartIndex, //--------------------------
				chordalBaseIndex + chordalAxis.size() + 2, //chordal
				chordalBaseIndex + chordalAxis.size() + 3, //nextChordal1
				references[diagonal.second].front
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 4> chordalVertices = {
				getChordalVertex(diagonal.first, chordalPos),		//0
				getChordalVertex(diagonal.first, nextChordal1Pos),	//1

				getChordalVertex(diagonal.second, chordalPos),		//2
				getChordalVertex(diagonal.second, nextChordal1Pos)	//3
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		case TriangleSideFlags::RIGHT: {
			//This diagonal has another adjacent diagonal
			//Draw 3 triangles:
			//
			//             X bestVertex
			//            /|\
			//           / | \
			//          /  |  X nextChordal0
			//         /   | / \
			//        /    |/   \
			//second X-----X-----X first <-current diagonal
			//         chordal
			//
			assert(preDiagonal.second == bestVertex);

			//Create the three triangles and add them
			const std::array<index_type, 8> triangles = {
				references[diagonal.first].front,
				chordalBaseIndex + chordalAxis.size() + 0, //nextChordal0
				chordalBaseIndex + chordalAxis.size() + 1, //chordal
				restartIndex, //--------------------------
				chordalBaseIndex + chordalAxis.size() + 2, //nextChordal0
				references[bestVertex].front,
				chordalBaseIndex + chordalAxis.size() + 3, //chordal
				references[diagonal.second].back
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 4> chordalVertices = {
				getChordalVertex(diagonal.first, nextChordal0Pos),	//0
				getChordalVertex(diagonal.first, chordalPos),		//1

				getChordalVertex(bestVertex, nextChordal0Pos),		//2
				getChordalVertex(bestVertex, chordalPos)			//3
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		case TriangleSideFlags::BOTH: {
			//This triangle has another 2 adjacent diagonals.
			//There is only one possible ordering
			//Draw 4 triangles:
			//
			//                X bestVertex
			//               / \
			//              /   \
			//nextChordal1 X-----X nextChordal0
			//            / \ X / \ <--- centroid
			//           /   \|/   \
			//   second X-----X-----X first <-current diagonal
			//             chordal
			//

			//Create the three triangles and add them
			const std::array<index_type, 14> triangles = {
				references[diagonal.first].front,
				chordalBaseIndex + chordalAxis.size() + 0,	//nextChordal0
				chordalBaseIndex + chordalAxis.size() + 1,	//chordal
				chordalBaseIndex + chordalAxis.size() + 2,	//centroid
				restartIndex, //--------------------------
				references[bestVertex].front,
				chordalBaseIndex + chordalAxis.size() + 3,	//nextChordal1
				chordalBaseIndex + chordalAxis.size() + 4,	//nextChordal0
				chordalBaseIndex + chordalAxis.size() + 5,	//centroid
				restartIndex, //--------------------------
				references[diagonal.second].front,
				chordalBaseIndex + chordalAxis.size() + 6,	//chordal
				chordalBaseIndex + chordalAxis.size() + 7,	//nextChordal1
				chordalBaseIndex + chordalAxis.size() + 8	//centroid
			};
			indices.insert(indices.cend(), triangles.cbegin(), triangles.cend());

			//Calculate the chordal vertices and add them
			const std::array<vertex_type, 9> chordalVertices = {
				getChordalVertex(diagonal.first, nextChordal0Pos),		//0
				getChordalVertex(diagonal.first, chordalPos),			//1
				getChordalVertex(diagonal.first, centroid),				//2

				getChordalVertex(bestVertex, nextChordal1Pos),			//3
				getChordalVertex(bestVertex, nextChordal0Pos),			//4
				getChordalVertex(bestVertex, centroid),					//5

				getChordalVertex(diagonal.second, chordalPos),			//6
				getChordalVertex(diagonal.second, nextChordal1Pos),		//7
				getChordalVertex(diagonal.second, centroid)				//8
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		default: {
			assert(!"No way!, we got here");
			//In a release version, at least, draw a tri ignoring the
			//chordal axis
			const std::array<index_type, 3> triangle = {
				chordalBaseIndex + chordalAxis.size() + 0,
				chordalBaseIndex + chordalAxis.size() + 1,
				chordalBaseIndex + chordalAxis.size() + 2
			};
			indices.insert(indices.cend(), triangle.cbegin(), triangle.cend());

			const std::array<vertex_type, 3> chordalVertices = {
				vertex_type(diagonalFirstPos, 	klm_vector_type(-1)),
				vertex_type(bestVertexPos,		klm_vector_type(-1)),
				vertex_type(diagonalSecondPos,	klm_vector_type(-1))
			};
			chordalAxis.insert(chordalAxis.cend(), chordalVertices.cbegin(), chordalVertices.cend());

			break;
		}
		}
	}

	return TriangleSideFlags::NONE; //We dont care about the next triangle 
}


template<typename T, typename I>
inline typename ChordalTriangulator<T, I>::vertex_type
ChordalTriangulator<T, I>::getChordalVertex(index_type index,
											const position_vector_type& pos ) const noexcept
{
	vertex_type result(pos, klm_vector_type(-1));
	if(m_references[index].helper < m_vertices.size()) {
		//Obtain the vertices
		const auto nextIndex = (index + 1) % m_references.size();
		const auto& helperVertex = m_vertices[m_references[index].helper];
		const auto& firstVertex = m_vertices[m_references[index].front];
		const auto& secondVertex = m_vertices[m_references[nextIndex].back];
		
		//Extrapolate the klm coordinates
		result.klm = extrapolate(
			firstVertex,
			helperVertex,
			secondVertex,
			pos
		);
	}

	return result;
}


template<typename T, typename I>
constexpr typename ChordalTriangulator<T, I>::klm_vector_type 
ChordalTriangulator<T, I>::extrapolate(	const vertex_type& v0,	
										const vertex_type& v1,
										const vertex_type& v2,
										const position_vector_type& pos ) noexcept
{
	const auto uv = getBarycentricCoordinates(v0.pos, v1.pos, v2.pos, pos);
	const auto w0 = 1 - uv.x - uv.y;
	const auto w1 = uv.x;
	const auto w2 = uv.y;
	return w0*v0.klm + w1*v1.klm + w2*v2.klm;
}

}
